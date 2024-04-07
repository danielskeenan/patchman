/**
 * @file updater.h
 *
 * @author Dan Keenan
 * @date 4/7/2024
 * @copyright GNU GPLv3
 */

#ifndef UPDATER_H
#define UPDATER_H

namespace patchman
{

/**
 * Setup the application self-updater.
 *
 * There are different implementations of this function per-platform.
 */
void setupUpdater();

/**
 * Force checking for updates.
 */
void checkForUpdates();

/**
 * Perform any updater cleanup needed.
 */
void cleanupUpdater();

} // patchman

#endif //UPDATER_H
