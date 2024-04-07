import base64
import hashlib
import http.client
import json.decoder
import mimetypes
import re
import shutil
import sys
import tempfile
import urllib.request
import urllib.response
import xml.etree.ElementTree as ET
from argparse import ArgumentParser
from datetime import datetime
from pathlib import Path
from typing import Optional

from cryptography.hazmat.primitives.asymmetric.dsa import DSAPrivateKey
from cryptography.hazmat.primitives.asymmetric.utils import Prehashed
from cryptography.hazmat.primitives.hashes import SHA1
from cryptography.hazmat.primitives.serialization import load_pem_private_key

# Parse link header
# https://docs.github.com/en/rest/using-the-rest-api/using-pagination-in-the-rest-api?apiVersion=2022-11-28#using-link-headers
GH_NEXT_RE = re.compile(r'(?<=<)([\S]*)(?=>; rel="Next")', flags=re.IGNORECASE)

VERSION_RE = re.compile(r'v?([a-z0-9.]+)$', flags=re.IGNORECASE)

RELEASE_REGEXES = {
    'windows-x64': re.compile(r'^.+-win64\.msi$', flags=re.IGNORECASE)
}


def handle_error(r: http.client.HTTPResponse, msg: str):
    if r.status != 200:
        raise RuntimeError(f'Error {r.status} {r.reason} ({msg})')


def get_pages(q: urllib.request.Request, msg: str, max_results: int = sys.maxsize, results=None):
    if results is None:
        results = []

    with urllib.request.urlopen(q) as r:
        handle_error(r, msg)
        results.extend(json.load(r))

    link = r.headers.get('link', failobj=None)
    if len(results) < max_results and link is not None and (match := re.match(GH_NEXT_RE, link)) is not None:
        next_q = urllib.request.Request(match.group(1), headers=q.headers)
        get_pages(next_q, msg, max_results, results)

    return results[0:max_results]


def main():
    argparser = ArgumentParser(description="Create an appcast xml file from github releases.")
    argparser.add_argument("--token", required=True, help="Github API token")
    argparser.add_argument("--repo", required=True, help="Repository to reference (as OWNER/REPO)")
    argparser.add_argument("--limit", type=int, default=5, help="Number of releases to include")
    argparser.add_argument("--dsa", help="Path to DSA Private Key")
    argparser.add_argument("out", help="Output XML file")
    args = argparser.parse_args()
    token = args.token
    repo = args.repo
    if args.dsa is not None:
        with open(args.dsa, mode='rb') as dsa_key_f:
            dsa_key: Optional[DSAPrivateKey] = load_pem_private_key(dsa_key_f.read(), None)
            if not isinstance(dsa_key, DSAPrivateKey):
                raise RuntimeError(f'{args.dsa} is not a DSA Private Key')
    else:
        dsa_key = None

    headers = {
        'Accept': 'application/vnd.github+json',
        'Authorization': f'Bearer {token}',
        'X-GitHub-Api-Version': '2022-11-28',
    }

    def download_asset(asset: dict, out_file):
        q = urllib.request.Request(f'https://api.github.com/repos/{repo}/releases/assets/{asset["id"]}',
                                   headers=headers | {'Accept': 'application/octet-stream'})
        with urllib.request.urlopen(q) as r:
            handle_error(r, f'downloading asset {asset["name"]}')
            shutil.copyfileobj(r, out_file)
            out_file.flush()

    def dsa_signature(in_file):
        digest = hashlib.file_digest(in_file, 'sha1').digest()
        signed = dsa_key.sign(digest, Prehashed(SHA1()))
        return str(base64.b64encode(signed), encoding='ascii')

    # Get repo
    q = urllib.request.Request(f'https://api.github.com/repos/{repo}', headers=headers)
    with urllib.request.urlopen(q) as r:
        handle_error(r, 'fetch repo')
        results = json.load(r)
        repo_name = results['name']
        repo_description = results['description']

    # Start the appcast file.
    tree = ET.ElementTree(ET.Element('rss'))
    rss = tree.getroot()
    rss.set('version', '2.0')
    rss.set('xmlns:sparkle', 'http://www.andymatuschak.org/xml-namespaces/sparkle')
    channel = ET.SubElement(rss, 'channel')
    ET.SubElement(channel, 'title').text = repo_name
    ET.SubElement(channel, 'description').text = repo_description
    ET.SubElement(channel, 'language').text = 'en'

    # Get list of releases.
    q = urllib.request.Request(f'https://api.github.com/repos/{repo}/releases', headers=headers)
    releases = get_pages(q, 'fetch releases', args.limit)
    for release in releases:
        if release['draft']:
            # Skip drafts.
            continue

        # Parse version number.
        if (match := re.match(VERSION_RE, release['tag_name'])) is None:
            continue
        version = match.group(1)

        # Parse release date.
        for date_key in ('created_at', 'published_at'):
            release[date_key] = datetime.fromisoformat(release[date_key])

        # Description
        q = urllib.request.Request(f'https://api.github.com/markdown', headers=headers)
        q.data = bytes(json.dumps({
            'text': release['body'],
            'mode': 'gfm',
            'context': repo
        }), encoding='utf-8')
        q.method = 'POST'
        q.add_header('Content-Type', 'text/markdown')
        with urllib.request.urlopen(q) as r:
            handle_error(r, 'asking for parsed markdown')
            release['body'] = str(r.read(), encoding='utf-8')

        # Check assets for ones valid for updating.
        for asset in release['assets']:
            # What kind of asset is this?
            for (platform, regex) in RELEASE_REGEXES.items():
                if re.match(regex, asset['name']) is not None:
                    item = ET.SubElement(channel, 'item')
                    ET.SubElement(item, 'title').text = release['name']
                    if release['prerelease']:
                        ET.SubElement(item, 'sparkle:channel').text = 'dev'
                    ET.SubElement(item, 'sparkle:fullReleaseNotesLink').text = release['html_url']
                    ET.SubElement(item, 'sparkle:version').text = version
                    ET.SubElement(item, 'pubDate').text = release['published_at'].strftime('%a, %d %b %Y %H:%M:%S %z')
                    ET.SubElement(item, 'sparkle:os').text = platform
                    ET.SubElement(item, 'description').text = release['body']
                    enclosure = ET.SubElement(item, 'enclosure')
                    enclosure.set('url', asset['browser_download_url'])
                    enclosure.set('sparkle:version', version)
                    enclosure.set('sparkle:shortVersionString', version)
                    enclosure.set('length', str(asset['size']))
                    if Path(asset['name']).suffix == '.msi':
                        # Unattended MSI install.
                        ET.SubElement(item, 'sparkle:installerArguments').text = '/passive'
                    # Need the actual file for the next steps.
                    with tempfile.NamedTemporaryFile(suffix=Path(asset['name']).suffix) as f:
                        download_asset(asset, f)
                        f.seek(0)
                        # Mime Type
                        (mimetype, _) = mimetypes.guess_type(f.name)
                        if mimetype is None:
                            mimetype = 'application/octet-stream'
                        enclosure.set('type', mimetype)
                        # DSA Signature
                        if dsa_key is not None:
                            dsa = dsa_signature(f)
                            enclosure.set('sparkle:dsaSignature', dsa)

    tree.write(args.out, encoding='utf-8', xml_declaration=True)
    print()


if __name__ == '__main__':
    main()
