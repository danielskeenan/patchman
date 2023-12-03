/**
 * @file D192Test.cpp
 *
 * @author Dan Keenan
 * @date 12/3/23
 * @copyright GNU GPLv3
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <array>
#include <QByteArrayView>
#include "patchlib/Rom.h"
#include "patchlib/Exceptions.h"
#include "Formatters.h"

using Catch::Matchers::RangeEquals;

static const std::array<unsigned char, 2048> kPatchBin
    {
        0x00, 0x01, 0x40, 0x41, 0x80, 0x81,
        0x02, 0x03, 0x42, 0x43, 0x82, 0x83,
        0x04, 0x05, 0x44, 0x45, 0x84, 0x85,
        0x06, 0x07, 0x46, 0x47, 0x86, 0x87,
        0x08, 0x09, 0x48, 0x49, 0x88, 0x89,
        0x0A, 0x0B, 0x4A, 0x4B, 0x8A, 0x8B,
        0x0C, 0x0D, 0x4C, 0x4D, 0x8C, 0x8D,
        0x0E, 0x0F, 0x4E, 0x4F, 0x8E, 0x8F,
        0x10, 0x11, 0x50, 0x51, 0x90, 0x91,
        0x12, 0x13, 0x52, 0x53, 0x92, 0x93,
        0x14, 0x15, 0x54, 0x55, 0x94, 0x95,
        0x16, 0x17, 0x56, 0x57, 0x96, 0x97,
        0x18, 0x19, 0x58, 0x59, 0x98, 0x99,
        0x1A, 0x1B, 0x5A, 0x5B, 0x9A, 0x9B,
        0x1C, 0x1D, 0x5C, 0x5D, 0x9C, 0x9D,
        0x1E, 0x1F, 0x5E, 0x5F, 0x9E, 0x9F,
        0x20, 0x21, 0x60, 0x61, 0xA0, 0xA1,
        0x22, 0x23, 0x62, 0x63, 0xA2, 0xA3,
        0x24, 0x25, 0x64, 0x65, 0xA4, 0xA5,
        0x26, 0x27, 0x66, 0x67, 0xA6, 0xA7,
        0x28, 0x29, 0x68, 0x69, 0xA8, 0xA9,
        0x2A, 0x2B, 0x6A, 0x6B, 0xAA, 0xAB,
        0x2C, 0x2D, 0x6C, 0x6D, 0xAC, 0xAD,
        0x2E, 0x2F, 0x6E, 0x6F, 0xAE, 0xAF,
        0x30, 0x31, 0x70, 0x71, 0xB0, 0xB1,
        0x32, 0x33, 0x72, 0x73, 0xB2, 0xB3,
        0x34, 0x35, 0x74, 0x75, 0xB4, 0xB5,
        0x36, 0x37, 0x76, 0x77, 0xB6, 0xB7,
        0x38, 0x39, 0x78, 0x79, 0xB8, 0xB9,
        0x3A, 0x3B, 0x7A, 0x7B, 0xBA, 0xBB,
        0x3C, 0x3D, 0x7C, 0x7D, 0xBC, 0xBD,
        0x3E, 0x3F, 0x7E, 0x7F, 0xBE, 0xBF,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0x00, 0x01, 0x40, 0x41, 0x80, 0x81,
        0x02, 0x03, 0x42, 0x43, 0x82, 0x83,
        0x04, 0x05, 0x44, 0x45, 0x84, 0x85,
        0x06, 0x07, 0x46, 0x47, 0x86, 0x87,
        0x08, 0x09, 0x48, 0x49, 0x88, 0x89,
        0x0A, 0x0B, 0x4A, 0x4B, 0x8A, 0x8B,
        0x0C, 0x0D, 0x4C, 0x4D, 0x8C, 0x8D,
        0x0E, 0x0F, 0x4E, 0x4F, 0x8E, 0x8F,
        0x10, 0x11, 0x50, 0x51, 0x90, 0x91,
        0x12, 0x13, 0x52, 0x53, 0x92, 0x93,
        0x14, 0x15, 0x54, 0x55, 0x94, 0x95,
        0x16, 0x17, 0x56, 0x57, 0x96, 0x97,
        0x18, 0x19, 0x58, 0x59, 0x98, 0x99,
        0x1A, 0x1B, 0x5A, 0x5B, 0x9A, 0x9B,
        0x1C, 0x1D, 0x5C, 0x5D, 0x9C, 0x9D,
        0x1E, 0x1F, 0x5E, 0x5F, 0x9E, 0x9F,
        0x20, 0x21, 0x60, 0x61, 0xA0, 0xA1,
        0x22, 0x23, 0x62, 0x63, 0xA2, 0xA3,
        0x24, 0x25, 0x64, 0x65, 0xA4, 0xA5,
        0x26, 0x27, 0x66, 0x67, 0xA6, 0xA7,
        0x28, 0x29, 0x68, 0x69, 0xA8, 0xA9,
        0x2A, 0x2B, // Out of address space!

        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0x00, 0x01, 0x40, 0x41, 0x80, 0x81,
        0x02, 0x03, 0x42, 0x43, 0x82, 0x83,
        0x04, 0x05, 0x44, 0x45, 0x84, 0x85,
        0x06, 0x07, 0x46, 0x47, 0x86, 0x87,
        0x08, 0x09, 0x48, 0x49, 0x88, 0x89,
        0x0A, 0x0B, 0x4A, 0x4B, 0x8A, 0x8B,
        0x0C, 0x0D, 0x4C, 0x4D, 0x8C, 0x8D,
        0x0E, 0x0F, 0x4E, 0x4F, 0x8E, 0x8F,
        0x10, 0x11, 0x50, 0x51, 0x90, 0x91,
        0x12, 0x13, 0x52, 0x53, 0x92, 0x93,
        0x14, 0x15, 0x54, 0x55, 0x94, 0x95,
        0x16, 0x17, 0x56, 0x57, 0x96, 0x97,
        0x18, 0x19, 0x58, 0x59, 0x98, 0x99,
        0x1A, 0x1B, 0x5A, 0x5B, 0x9A, 0x9B,
        0x1C, 0x1D, 0x5C, 0x5D, 0x9C, 0x9D,
        0x1E, 0x1F, 0x5E, 0x5F, 0x9E, 0x9F,
        0x20, 0x21, 0x60, 0x61, 0xA0, 0xA1,
        0x22, 0x23, 0x62, 0x63, 0xA2, 0xA3,
        0x24, 0x25, 0x64, 0x65, 0xA4, 0xA5,
        0x26, 0x27, 0x66, 0x67, 0xA6, 0xA7,
        0x28, 0x29, 0x68, 0x69, 0xA8, 0xA9,
        0x2A, 0x2B, 0x6A, 0x6B, 0xAA, 0xAB,
        0x2C, 0x2D, 0x6C, 0x6D, 0xAC, 0xAD,
        0x2E, 0x2F, 0x6E, 0x6F, 0xAE, 0xAF,
        0x30, 0x31, 0x70, 0x71, 0xB0, 0xB1,
        0x32, 0x33, 0x72, 0x73, 0xB2, 0xB3,
        0x34, 0x35, 0x74, 0x75, 0xB4, 0xB5,
        0x36, 0x37, 0x76, 0x77, 0xB6, 0xB7,
        0x38, 0x39, 0x78, 0x79, 0xB8, 0xB9,
        0x3A, 0x3B, 0x7A, 0x7B, 0xBA, 0xBB,
        0x3C, 0x3D, 0x7C, 0x7D, 0xBC, 0xBD,
        0x3E, 0x3F, 0x7E, 0x7F, 0xBE, 0xBF,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

        0x00, 0x01, 0x40, 0x41, 0x80, 0x81,  // 1-6
        0x02, 0x03, 0x42, 0x43, 0x82, 0x83,  // 7-12
        0x04, 0x05, 0x44, 0x45, 0x84, 0x85,  // 13-18
        0x06, 0x07, 0x46, 0x47, 0x86, 0x87,  // 19-24
        0x08, 0x09, 0x48, 0x49, 0x88, 0x89,  // 25-30
        0x0A, 0x0B, 0x4A, 0x4B, 0x8A, 0x8B,  // 31-36
        0x0C, 0x0D, 0x4C, 0x4D, 0x8C, 0x8D,  // 37-42
        0x0E, 0x0F, 0x4E, 0x4F, 0x8E, 0x8F,  // 43-48
        0x10, 0x11, 0x50, 0x51, 0x90, 0x91,  // 49-54
        0x12, 0x13, 0x52, 0x53, 0x92, 0x93,  // 55-60
        0x14, 0x15, 0x54, 0x55, 0x94, 0x95,  // 61-66
        0x16, 0x17, 0x56, 0x57, 0x96, 0x97,  // 67-72
        0x18, 0x19, 0x58, 0x59, 0x98, 0x99,  // 73-78
        0x1A, 0x1B, 0x5A, 0x5B, 0x9A, 0x9B,  // 79-84
        0x1C, 0x1D, 0x5C, 0x5D, 0x9C, 0x9D,  // 85-90
        0x1E, 0x1F, 0x5E, 0x5F, 0x9E, 0x9F,  // 91-96
        0x20, 0x21, 0x60, 0x61, 0xA0, 0xA1,  // 97-102
        0x22, 0x23, 0x62, 0x63, 0xA2, 0xA3,  // 103-108
        0x24, 0x25, 0x64, 0x65, 0xA4, 0xA5,  // 109-114
        0x26, 0x27, 0x66, 0x67, 0xA6, 0xA7,  // 115-120
        0x28, 0x29, 0x68, 0x69, 0xA8, 0xA9,  // 121-126
        0x2A, 0x2B, 0x6A, 0x6B, 0xAA, 0xAB,  // 127-132
        0x2C, 0x2D, 0x6C, 0x6D, 0xAC, 0xAD,  // 133-138
        0x2E, 0x2F, 0x6E, 0x6F, 0xAE, 0xAF,  // 139-144
        0x30, 0x31, 0x70, 0x71, 0xB0, 0xB1,  // 145-150
        0x32, 0x33, 0x72, 0x73, 0xB2, 0xB3,  // 151-156
        0x34, 0x35, 0x74, 0x75, 0xB4, 0xB5,  // 157-162
        0x36, 0x37, 0x76, 0x77, 0xB6, 0xB7,  // 163-168
        0x38, 0x39, 0x78, 0x79, 0xB8, 0xB9,  // 169-174
        0x3A, 0x3B, 0x7A, 0x7B, 0xBA, 0xBB,  // 175-180
        0x3C, 0x3D, 0x7C, 0x7D, 0x3E, 0x7E,  // 181-186
        0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 187-192
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };

static const patchlib::Rom *kTestRom = []()
{
    patchlib::Rom *rom = patchlib::Rom::create(patchlib::Rom::Type::D192);

    patchlib::Rack *rack;
    rack = rom->addRack(0, patchlib::Rack::Type::D192Rack);
    rack->setLugAddress(0, 1);
    rack->setLugAddress(1, 2);
    rack->setLugAddress(2, 7);
    rack->setLugAddress(3, 8);
    rack->setLugAddress(4, 13);
    rack->setLugAddress(5, 14);
    rack->setLugAddress(6, 19);
    rack->setLugAddress(7, 20);
    rack->setLugAddress(8, 25);
    rack->setLugAddress(9, 26);
    rack->setLugAddress(10, 31);
    rack->setLugAddress(11, 32);
    rack->setLugAddress(12, 37);
    rack->setLugAddress(13, 38);
    rack->setLugAddress(14, 43);
    rack->setLugAddress(15, 44);
    rack->setLugAddress(16, 49);
    rack->setLugAddress(17, 50);
    rack->setLugAddress(18, 55);
    rack->setLugAddress(19, 56);
    rack->setLugAddress(20, 61);
    rack->setLugAddress(21, 62);
    rack->setLugAddress(22, 67);
    rack->setLugAddress(23, 68);
    rack->setLugAddress(24, 73);
    rack->setLugAddress(25, 74);
    rack->setLugAddress(26, 79);
    rack->setLugAddress(27, 80);
    rack->setLugAddress(28, 85);
    rack->setLugAddress(29, 86);
    rack->setLugAddress(30, 91);
    rack->setLugAddress(31, 92);
    rack->setLugAddress(32, 97);
    rack->setLugAddress(33, 98);
    rack->setLugAddress(34, 103);
    rack->setLugAddress(35, 104);
    rack->setLugAddress(36, 109);
    rack->setLugAddress(37, 110);
    rack->setLugAddress(38, 115);
    rack->setLugAddress(39, 116);
    rack->setLugAddress(40, 121);
    rack->setLugAddress(41, 122);
    rack->setLugAddress(42, 127);
    rack->setLugAddress(43, 128);
    rack->setLugAddress(44, 133);
    rack->setLugAddress(45, 134);
    rack->setLugAddress(46, 139);
    rack->setLugAddress(47, 140);
    rack->setLugAddress(48, 145);
    rack->setLugAddress(49, 146);
    rack->setLugAddress(50, 151);
    rack->setLugAddress(51, 152);
    rack->setLugAddress(52, 157);
    rack->setLugAddress(53, 158);
    rack->setLugAddress(54, 163);
    rack->setLugAddress(55, 164);
    rack->setLugAddress(56, 169);
    rack->setLugAddress(57, 170);
    rack->setLugAddress(58, 175);
    rack->setLugAddress(59, 176);
    rack->setLugAddress(60, 181);
    rack->setLugAddress(61, 182);
    rack->setLugAddress(62, 185);
    rack->setLugAddress(63, 0);

    rack->setLugAddress(64, 3);
    rack->setLugAddress(65, 4);
    rack->setLugAddress(66, 9);
    rack->setLugAddress(67, 10);
    rack->setLugAddress(68, 15);
    rack->setLugAddress(69, 16);
    rack->setLugAddress(70, 21);
    rack->setLugAddress(71, 22);
    rack->setLugAddress(72, 27);
    rack->setLugAddress(73, 28);
    rack->setLugAddress(74, 33);
    rack->setLugAddress(75, 34);
    rack->setLugAddress(76, 39);
    rack->setLugAddress(77, 40);
    rack->setLugAddress(78, 45);
    rack->setLugAddress(79, 46);
    rack->setLugAddress(80, 51);
    rack->setLugAddress(81, 52);
    rack->setLugAddress(82, 57);
    rack->setLugAddress(83, 58);
    rack->setLugAddress(84, 63);
    rack->setLugAddress(85, 64);
    rack->setLugAddress(86, 69);
    rack->setLugAddress(87, 70);
    rack->setLugAddress(88, 75);
    rack->setLugAddress(89, 76);
    rack->setLugAddress(90, 81);
    rack->setLugAddress(91, 82);
    rack->setLugAddress(92, 87);
    rack->setLugAddress(93, 88);
    rack->setLugAddress(94, 93);
    rack->setLugAddress(95, 94);
    rack->setLugAddress(96, 99);
    rack->setLugAddress(97, 100);
    rack->setLugAddress(98, 105);
    rack->setLugAddress(99, 106);
    rack->setLugAddress(100, 111);
    rack->setLugAddress(101, 112);
    rack->setLugAddress(102, 117);
    rack->setLugAddress(103, 118);
    rack->setLugAddress(104, 123);
    rack->setLugAddress(105, 124);
    rack->setLugAddress(106, 129);
    rack->setLugAddress(107, 130);
    rack->setLugAddress(108, 135);
    rack->setLugAddress(109, 136);
    rack->setLugAddress(110, 141);
    rack->setLugAddress(111, 142);
    rack->setLugAddress(112, 147);
    rack->setLugAddress(113, 148);
    rack->setLugAddress(114, 153);
    rack->setLugAddress(115, 154);
    rack->setLugAddress(116, 159);
    rack->setLugAddress(117, 160);
    rack->setLugAddress(118, 165);
    rack->setLugAddress(119, 166);
    rack->setLugAddress(120, 171);
    rack->setLugAddress(121, 172);
    rack->setLugAddress(122, 177);
    rack->setLugAddress(123, 178);
    rack->setLugAddress(124, 183);
    rack->setLugAddress(125, 184);
    rack->setLugAddress(126, 186);
    rack->setLugAddress(127, 0);

    rack->setLugAddress(128, 5);
    rack->setLugAddress(129, 6);
    rack->setLugAddress(130, 11);
    rack->setLugAddress(131, 12);
    rack->setLugAddress(132, 17);
    rack->setLugAddress(133, 18);
    rack->setLugAddress(134, 23);
    rack->setLugAddress(135, 24);
    rack->setLugAddress(136, 29);
    rack->setLugAddress(137, 30);
    rack->setLugAddress(138, 35);
    rack->setLugAddress(139, 36);
    rack->setLugAddress(140, 41);
    rack->setLugAddress(141, 42);
    rack->setLugAddress(142, 47);
    rack->setLugAddress(143, 48);
    rack->setLugAddress(144, 53);
    rack->setLugAddress(145, 54);
    rack->setLugAddress(146, 59);
    rack->setLugAddress(147, 60);
    rack->setLugAddress(148, 65);
    rack->setLugAddress(149, 66);
    rack->setLugAddress(150, 71);
    rack->setLugAddress(151, 72);
    rack->setLugAddress(152, 77);
    rack->setLugAddress(153, 78);
    rack->setLugAddress(154, 83);
    rack->setLugAddress(155, 84);
    rack->setLugAddress(156, 89);
    rack->setLugAddress(157, 90);
    rack->setLugAddress(158, 95);
    rack->setLugAddress(159, 96);
    rack->setLugAddress(160, 101);
    rack->setLugAddress(161, 102);
    rack->setLugAddress(162, 107);
    rack->setLugAddress(163, 108);
    rack->setLugAddress(164, 113);
    rack->setLugAddress(165, 114);
    rack->setLugAddress(166, 119);
    rack->setLugAddress(167, 120);
    rack->setLugAddress(168, 125);
    rack->setLugAddress(169, 126);
    rack->setLugAddress(170, 131);
    rack->setLugAddress(171, 132);
    rack->setLugAddress(172, 137);
    rack->setLugAddress(173, 138);
    rack->setLugAddress(174, 143);
    rack->setLugAddress(175, 144);
    rack->setLugAddress(176, 149);
    rack->setLugAddress(177, 150);
    rack->setLugAddress(178, 155);
    rack->setLugAddress(179, 156);
    rack->setLugAddress(180, 161);
    rack->setLugAddress(181, 162);
    rack->setLugAddress(182, 167);
    rack->setLugAddress(183, 168);
    rack->setLugAddress(184, 173);
    rack->setLugAddress(185, 174);
    rack->setLugAddress(186, 179);
    rack->setLugAddress(187, 180);
    rack->setLugAddress(188, 0);
    rack->setLugAddress(189, 0);
    rack->setLugAddress(190, 187);
    rack->setLugAddress(191, 0);

    rack = rom->addRack(1, patchlib::Rack::Type::D192Rack);
    rack->setLugAddress(0, 193);
    rack->setLugAddress(1, 194);
    rack->setLugAddress(2, 199);
    rack->setLugAddress(3, 200);
    rack->setLugAddress(4, 205);
    rack->setLugAddress(5, 206);
    rack->setLugAddress(6, 211);
    rack->setLugAddress(7, 212);
    rack->setLugAddress(8, 217);
    rack->setLugAddress(9, 218);
    rack->setLugAddress(10, 223);
    rack->setLugAddress(11, 224);
    rack->setLugAddress(12, 229);
    rack->setLugAddress(13, 230);
    rack->setLugAddress(14, 235);
    rack->setLugAddress(15, 236);
    rack->setLugAddress(16, 241);
    rack->setLugAddress(17, 242);
    rack->setLugAddress(18, 247);
    rack->setLugAddress(19, 248);
    rack->setLugAddress(20, 253);
    rack->setLugAddress(21, 254);
    rack->setLugAddress(22, 259);
    rack->setLugAddress(23, 260);
    rack->setLugAddress(24, 265);
    rack->setLugAddress(25, 266);
    rack->setLugAddress(26, 271);
    rack->setLugAddress(27, 272);
    rack->setLugAddress(28, 277);
    rack->setLugAddress(29, 278);
    rack->setLugAddress(30, 283);
    rack->setLugAddress(31, 284);
    rack->setLugAddress(32, 289);
    rack->setLugAddress(33, 290);
    rack->setLugAddress(34, 295);
    rack->setLugAddress(35, 296);
    rack->setLugAddress(36, 301);
    rack->setLugAddress(37, 302);
    rack->setLugAddress(38, 307);
    rack->setLugAddress(39, 308);
    rack->setLugAddress(40, 313);
    rack->setLugAddress(41, 314);
    rack->setLugAddress(42, 319);
    rack->setLugAddress(43, 320);
    rack->setLugAddress(44, 325);
    rack->setLugAddress(45, 326);
    rack->setLugAddress(46, 331);
    rack->setLugAddress(47, 332);
    rack->setLugAddress(48, 337);
    rack->setLugAddress(49, 338);
    rack->setLugAddress(50, 343);
    rack->setLugAddress(51, 344);
    rack->setLugAddress(52, 349);
    rack->setLugAddress(53, 350);
    rack->setLugAddress(54, 355);
    rack->setLugAddress(55, 356);
    rack->setLugAddress(56, 361);
    rack->setLugAddress(57, 362);
    rack->setLugAddress(58, 367);
    rack->setLugAddress(59, 368);
    rack->setLugAddress(60, 373);
    rack->setLugAddress(61, 374);
    rack->setLugAddress(62, 379);
    rack->setLugAddress(63, 380);

    rack->setLugAddress(64, 195);
    rack->setLugAddress(65, 196);
    rack->setLugAddress(66, 201);
    rack->setLugAddress(67, 202);
    rack->setLugAddress(68, 207);
    rack->setLugAddress(69, 208);
    rack->setLugAddress(70, 213);
    rack->setLugAddress(71, 214);
    rack->setLugAddress(72, 219);
    rack->setLugAddress(73, 220);
    rack->setLugAddress(74, 225);
    rack->setLugAddress(75, 226);
    rack->setLugAddress(76, 231);
    rack->setLugAddress(77, 232);
    rack->setLugAddress(78, 237);
    rack->setLugAddress(79, 238);
    rack->setLugAddress(80, 243);
    rack->setLugAddress(81, 244);
    rack->setLugAddress(82, 249);
    rack->setLugAddress(83, 250);
    rack->setLugAddress(84, 255);
    rack->setLugAddress(85, 256);
    rack->setLugAddress(86, 261);
    rack->setLugAddress(87, 262);
    rack->setLugAddress(88, 267);
    rack->setLugAddress(89, 268);
    rack->setLugAddress(90, 273);
    rack->setLugAddress(91, 274);
    rack->setLugAddress(92, 279);
    rack->setLugAddress(93, 280);
    rack->setLugAddress(94, 285);
    rack->setLugAddress(95, 286);
    rack->setLugAddress(96, 291);
    rack->setLugAddress(97, 292);
    rack->setLugAddress(98, 297);
    rack->setLugAddress(99, 298);
    rack->setLugAddress(100, 303);
    rack->setLugAddress(101, 304);
    rack->setLugAddress(102, 309);
    rack->setLugAddress(103, 310);
    rack->setLugAddress(104, 315);
    rack->setLugAddress(105, 316);
    rack->setLugAddress(106, 321);
    rack->setLugAddress(107, 322);
    rack->setLugAddress(108, 327);
    rack->setLugAddress(109, 328);
    rack->setLugAddress(110, 333);
    rack->setLugAddress(111, 334);
    rack->setLugAddress(112, 339);
    rack->setLugAddress(113, 340);
    rack->setLugAddress(114, 345);
    rack->setLugAddress(115, 346);
    rack->setLugAddress(116, 351);
    rack->setLugAddress(117, 352);
    rack->setLugAddress(118, 357);
    rack->setLugAddress(119, 358);
    rack->setLugAddress(120, 363);
    rack->setLugAddress(121, 364);
    rack->setLugAddress(122, 369);
    rack->setLugAddress(123, 370);
    rack->setLugAddress(124, 375);
    rack->setLugAddress(125, 376);
    rack->setLugAddress(126, 381);
    rack->setLugAddress(127, 382);

    rack->setLugAddress(128, 197);
    rack->setLugAddress(129, 198);
    rack->setLugAddress(130, 203);
    rack->setLugAddress(131, 204);
    rack->setLugAddress(132, 209);
    rack->setLugAddress(133, 210);
    rack->setLugAddress(134, 215);
    rack->setLugAddress(135, 216);
    rack->setLugAddress(136, 221);
    rack->setLugAddress(137, 222);
    rack->setLugAddress(138, 227);
    rack->setLugAddress(139, 228);
    rack->setLugAddress(140, 233);
    rack->setLugAddress(141, 234);
    rack->setLugAddress(142, 239);
    rack->setLugAddress(143, 240);
    rack->setLugAddress(144, 245);
    rack->setLugAddress(145, 246);
    rack->setLugAddress(146, 251);
    rack->setLugAddress(147, 252);
    rack->setLugAddress(148, 257);
    rack->setLugAddress(149, 258);
    rack->setLugAddress(150, 263);
    rack->setLugAddress(151, 264);
    rack->setLugAddress(152, 269);
    rack->setLugAddress(153, 270);
    rack->setLugAddress(154, 275);
    rack->setLugAddress(155, 276);
    rack->setLugAddress(156, 281);
    rack->setLugAddress(157, 282);
    rack->setLugAddress(158, 287);
    rack->setLugAddress(159, 288);
    rack->setLugAddress(160, 293);
    rack->setLugAddress(161, 294);
    rack->setLugAddress(162, 299);
    rack->setLugAddress(163, 300);
    rack->setLugAddress(164, 305);
    rack->setLugAddress(165, 306);
    rack->setLugAddress(166, 311);
    rack->setLugAddress(167, 312);
    rack->setLugAddress(168, 317);
    rack->setLugAddress(169, 318);
    rack->setLugAddress(170, 323);
    rack->setLugAddress(171, 324);
    rack->setLugAddress(172, 329);
    rack->setLugAddress(173, 330);
    rack->setLugAddress(174, 335);
    rack->setLugAddress(175, 336);
    rack->setLugAddress(176, 341);
    rack->setLugAddress(177, 342);
    rack->setLugAddress(178, 347);
    rack->setLugAddress(179, 348);
    rack->setLugAddress(180, 353);
    rack->setLugAddress(181, 354);
    rack->setLugAddress(182, 359);
    rack->setLugAddress(183, 360);
    rack->setLugAddress(184, 365);
    rack->setLugAddress(185, 366);
    rack->setLugAddress(186, 371);
    rack->setLugAddress(187, 372);
    rack->setLugAddress(188, 377);
    rack->setLugAddress(189, 378);
    rack->setLugAddress(190, 383);
    rack->setLugAddress(191, 384);

    rack = rom->addRack(2, patchlib::Rack::Type::D192Rack);
    rack->setLugAddress(0, 385);
    rack->setLugAddress(1, 386);
    rack->setLugAddress(2, 391);
    rack->setLugAddress(3, 392);
    rack->setLugAddress(4, 397);
    rack->setLugAddress(5, 398);
    rack->setLugAddress(6, 403);
    rack->setLugAddress(7, 404);
    rack->setLugAddress(8, 409);
    rack->setLugAddress(9, 410);
    rack->setLugAddress(10, 415);
    rack->setLugAddress(11, 416);
    rack->setLugAddress(12, 421);
    rack->setLugAddress(13, 422);
    rack->setLugAddress(14, 427);
    rack->setLugAddress(15, 428);
    rack->setLugAddress(16, 433);
    rack->setLugAddress(17, 434);
    rack->setLugAddress(18, 439);
    rack->setLugAddress(19, 440);
    rack->setLugAddress(20, 445);
    rack->setLugAddress(21, 446);
    rack->setLugAddress(22, 451);
    rack->setLugAddress(23, 452);
    rack->setLugAddress(24, 457);
    rack->setLugAddress(25, 458);
    rack->setLugAddress(26, 463);
    rack->setLugAddress(27, 464);
    rack->setLugAddress(28, 469);
    rack->setLugAddress(29, 470);
    rack->setLugAddress(30, 475);
    rack->setLugAddress(31, 476);
    rack->setLugAddress(32, 481);
    rack->setLugAddress(33, 482);
    rack->setLugAddress(34, 487);
    rack->setLugAddress(35, 488);
    rack->setLugAddress(36, 493);
    rack->setLugAddress(37, 494);
    rack->setLugAddress(38, 499);
    rack->setLugAddress(39, 500);
    rack->setLugAddress(40, 505);
    rack->setLugAddress(41, 506);
    rack->setLugAddress(42, 511);
    rack->setLugAddress(43, 512);
    rack->setLugAddress(44, 0);
    rack->setLugAddress(45, 0);
    rack->setLugAddress(46, 0);
    rack->setLugAddress(47, 0);
    rack->setLugAddress(48, 0);
    rack->setLugAddress(49, 0);
    rack->setLugAddress(50, 0);
    rack->setLugAddress(51, 0);
    rack->setLugAddress(52, 0);
    rack->setLugAddress(53, 0);
    rack->setLugAddress(54, 0);
    rack->setLugAddress(55, 0);
    rack->setLugAddress(56, 0);
    rack->setLugAddress(57, 0);
    rack->setLugAddress(58, 0);
    rack->setLugAddress(59, 0);
    rack->setLugAddress(60, 0);
    rack->setLugAddress(61, 0);
    rack->setLugAddress(62, 0);
    rack->setLugAddress(63, 0);

    rack->setLugAddress(64, 387);
    rack->setLugAddress(65, 388);
    rack->setLugAddress(66, 393);
    rack->setLugAddress(67, 394);
    rack->setLugAddress(68, 399);
    rack->setLugAddress(69, 400);
    rack->setLugAddress(70, 405);
    rack->setLugAddress(71, 406);
    rack->setLugAddress(72, 411);
    rack->setLugAddress(73, 412);
    rack->setLugAddress(74, 417);
    rack->setLugAddress(75, 418);
    rack->setLugAddress(76, 423);
    rack->setLugAddress(77, 424);
    rack->setLugAddress(78, 429);
    rack->setLugAddress(79, 430);
    rack->setLugAddress(80, 435);
    rack->setLugAddress(81, 436);
    rack->setLugAddress(82, 441);
    rack->setLugAddress(83, 442);
    rack->setLugAddress(84, 447);
    rack->setLugAddress(85, 448);
    rack->setLugAddress(86, 453);
    rack->setLugAddress(87, 454);
    rack->setLugAddress(88, 459);
    rack->setLugAddress(89, 460);
    rack->setLugAddress(90, 465);
    rack->setLugAddress(91, 466);
    rack->setLugAddress(92, 471);
    rack->setLugAddress(93, 472);
    rack->setLugAddress(94, 477);
    rack->setLugAddress(95, 478);
    rack->setLugAddress(96, 483);
    rack->setLugAddress(97, 484);
    rack->setLugAddress(98, 489);
    rack->setLugAddress(99, 490);
    rack->setLugAddress(100, 495);
    rack->setLugAddress(101, 496);
    rack->setLugAddress(102, 501);
    rack->setLugAddress(103, 502);
    rack->setLugAddress(104, 507);
    rack->setLugAddress(105, 508);
    rack->setLugAddress(106, 0);
    rack->setLugAddress(107, 0);
    rack->setLugAddress(108, 0);
    rack->setLugAddress(109, 0);
    rack->setLugAddress(110, 0);
    rack->setLugAddress(111, 0);
    rack->setLugAddress(112, 0);
    rack->setLugAddress(113, 0);
    rack->setLugAddress(114, 0);
    rack->setLugAddress(115, 0);
    rack->setLugAddress(116, 0);
    rack->setLugAddress(117, 0);
    rack->setLugAddress(118, 0);
    rack->setLugAddress(119, 0);
    rack->setLugAddress(120, 0);
    rack->setLugAddress(121, 0);
    rack->setLugAddress(122, 0);
    rack->setLugAddress(123, 0);
    rack->setLugAddress(124, 0);
    rack->setLugAddress(125, 0);
    rack->setLugAddress(126, 0);
    rack->setLugAddress(127, 0);

    rack->setLugAddress(128, 389);
    rack->setLugAddress(129, 390);
    rack->setLugAddress(130, 395);
    rack->setLugAddress(131, 396);
    rack->setLugAddress(132, 401);
    rack->setLugAddress(133, 402);
    rack->setLugAddress(134, 407);
    rack->setLugAddress(135, 408);
    rack->setLugAddress(136, 413);
    rack->setLugAddress(137, 414);
    rack->setLugAddress(138, 419);
    rack->setLugAddress(139, 420);
    rack->setLugAddress(140, 425);
    rack->setLugAddress(141, 426);
    rack->setLugAddress(142, 431);
    rack->setLugAddress(143, 432);
    rack->setLugAddress(144, 437);
    rack->setLugAddress(145, 438);
    rack->setLugAddress(146, 443);
    rack->setLugAddress(147, 444);
    rack->setLugAddress(148, 449);
    rack->setLugAddress(149, 450);
    rack->setLugAddress(150, 455);
    rack->setLugAddress(151, 456);
    rack->setLugAddress(152, 461);
    rack->setLugAddress(153, 462);
    rack->setLugAddress(154, 467);
    rack->setLugAddress(155, 468);
    rack->setLugAddress(156, 473);
    rack->setLugAddress(157, 474);
    rack->setLugAddress(158, 479);
    rack->setLugAddress(159, 480);
    rack->setLugAddress(160, 485);
    rack->setLugAddress(161, 486);
    rack->setLugAddress(162, 491);
    rack->setLugAddress(163, 492);
    rack->setLugAddress(164, 497);
    rack->setLugAddress(165, 498);
    rack->setLugAddress(166, 503);
    rack->setLugAddress(167, 504);
    rack->setLugAddress(168, 509);
    rack->setLugAddress(169, 510);
    rack->setLugAddress(170, 0);
    rack->setLugAddress(171, 0);
    rack->setLugAddress(172, 0);
    rack->setLugAddress(173, 0);
    rack->setLugAddress(174, 0);
    rack->setLugAddress(175, 0);
    rack->setLugAddress(176, 0);
    rack->setLugAddress(177, 0);
    rack->setLugAddress(178, 0);
    rack->setLugAddress(179, 0);
    rack->setLugAddress(180, 0);
    rack->setLugAddress(181, 0);
    rack->setLugAddress(182, 0);
    rack->setLugAddress(183, 0);
    rack->setLugAddress(184, 0);
    rack->setLugAddress(185, 0);
    rack->setLugAddress(186, 0);
    rack->setLugAddress(187, 0);
    rack->setLugAddress(188, 0);
    rack->setLugAddress(189, 0);
    rack->setLugAddress(190, 0);
    rack->setLugAddress(191, 0);

    rack = rom->addRack(3, patchlib::Rack::Type::D192Rack);
    rack->setLugAddress(0, 1);
    rack->setLugAddress(1, 2);
    rack->setLugAddress(2, 7);
    rack->setLugAddress(3, 8);
    rack->setLugAddress(4, 13);
    rack->setLugAddress(5, 14);
    rack->setLugAddress(6, 19);
    rack->setLugAddress(7, 20);
    rack->setLugAddress(8, 25);
    rack->setLugAddress(9, 26);
    rack->setLugAddress(10, 31);
    rack->setLugAddress(11, 32);
    rack->setLugAddress(12, 37);
    rack->setLugAddress(13, 38);
    rack->setLugAddress(14, 43);
    rack->setLugAddress(15, 44);
    rack->setLugAddress(16, 49);
    rack->setLugAddress(17, 50);
    rack->setLugAddress(18, 55);
    rack->setLugAddress(19, 56);
    rack->setLugAddress(20, 61);
    rack->setLugAddress(21, 62);
    rack->setLugAddress(22, 67);
    rack->setLugAddress(23, 68);
    rack->setLugAddress(24, 73);
    rack->setLugAddress(25, 74);
    rack->setLugAddress(26, 79);
    rack->setLugAddress(27, 80);
    rack->setLugAddress(28, 85);
    rack->setLugAddress(29, 86);
    rack->setLugAddress(30, 91);
    rack->setLugAddress(31, 92);
    rack->setLugAddress(32, 97);
    rack->setLugAddress(33, 98);
    rack->setLugAddress(34, 103);
    rack->setLugAddress(35, 104);
    rack->setLugAddress(36, 109);
    rack->setLugAddress(37, 110);
    rack->setLugAddress(38, 115);
    rack->setLugAddress(39, 116);
    rack->setLugAddress(40, 121);
    rack->setLugAddress(41, 122);
    rack->setLugAddress(42, 127);
    rack->setLugAddress(43, 128);
    rack->setLugAddress(44, 133);
    rack->setLugAddress(45, 134);
    rack->setLugAddress(46, 139);
    rack->setLugAddress(47, 140);
    rack->setLugAddress(48, 145);
    rack->setLugAddress(49, 146);
    rack->setLugAddress(50, 151);
    rack->setLugAddress(51, 152);
    rack->setLugAddress(52, 157);
    rack->setLugAddress(53, 158);
    rack->setLugAddress(54, 163);
    rack->setLugAddress(55, 164);
    rack->setLugAddress(56, 169);
    rack->setLugAddress(57, 170);
    rack->setLugAddress(58, 175);
    rack->setLugAddress(59, 176);
    rack->setLugAddress(60, 181);
    rack->setLugAddress(61, 182);
    rack->setLugAddress(62, 187);
    rack->setLugAddress(63, 188);

    rack->setLugAddress(64, 3);
    rack->setLugAddress(65, 4);
    rack->setLugAddress(66, 9);
    rack->setLugAddress(67, 10);
    rack->setLugAddress(68, 15);
    rack->setLugAddress(69, 16);
    rack->setLugAddress(70, 21);
    rack->setLugAddress(71, 22);
    rack->setLugAddress(72, 27);
    rack->setLugAddress(73, 28);
    rack->setLugAddress(74, 33);
    rack->setLugAddress(75, 34);
    rack->setLugAddress(76, 39);
    rack->setLugAddress(77, 40);
    rack->setLugAddress(78, 45);
    rack->setLugAddress(79, 46);
    rack->setLugAddress(80, 51);
    rack->setLugAddress(81, 52);
    rack->setLugAddress(82, 57);
    rack->setLugAddress(83, 58);
    rack->setLugAddress(84, 63);
    rack->setLugAddress(85, 64);
    rack->setLugAddress(86, 69);
    rack->setLugAddress(87, 70);
    rack->setLugAddress(88, 75);
    rack->setLugAddress(89, 76);
    rack->setLugAddress(90, 81);
    rack->setLugAddress(91, 82);
    rack->setLugAddress(92, 87);
    rack->setLugAddress(93, 88);
    rack->setLugAddress(94, 93);
    rack->setLugAddress(95, 94);
    rack->setLugAddress(96, 99);
    rack->setLugAddress(97, 100);
    rack->setLugAddress(98, 105);
    rack->setLugAddress(99, 106);
    rack->setLugAddress(100, 111);
    rack->setLugAddress(101, 112);
    rack->setLugAddress(102, 117);
    rack->setLugAddress(103, 118);
    rack->setLugAddress(104, 123);
    rack->setLugAddress(105, 124);
    rack->setLugAddress(106, 129);
    rack->setLugAddress(107, 130);
    rack->setLugAddress(108, 135);
    rack->setLugAddress(109, 136);
    rack->setLugAddress(110, 141);
    rack->setLugAddress(111, 142);
    rack->setLugAddress(112, 147);
    rack->setLugAddress(113, 148);
    rack->setLugAddress(114, 153);
    rack->setLugAddress(115, 154);
    rack->setLugAddress(116, 159);
    rack->setLugAddress(117, 160);
    rack->setLugAddress(118, 165);
    rack->setLugAddress(119, 166);
    rack->setLugAddress(120, 171);
    rack->setLugAddress(121, 172);
    rack->setLugAddress(122, 177);
    rack->setLugAddress(123, 178);
    rack->setLugAddress(124, 183);
    rack->setLugAddress(125, 184);
    rack->setLugAddress(126, 189);
    rack->setLugAddress(127, 190);

    rack->setLugAddress(128, 5);
    rack->setLugAddress(129, 6);
    rack->setLugAddress(130, 11);
    rack->setLugAddress(131, 12);
    rack->setLugAddress(132, 17);
    rack->setLugAddress(133, 18);
    rack->setLugAddress(134, 23);
    rack->setLugAddress(135, 24);
    rack->setLugAddress(136, 29);
    rack->setLugAddress(137, 30);
    rack->setLugAddress(138, 35);
    rack->setLugAddress(139, 36);
    rack->setLugAddress(140, 41);
    rack->setLugAddress(141, 42);
    rack->setLugAddress(142, 47);
    rack->setLugAddress(143, 48);
    rack->setLugAddress(144, 53);
    rack->setLugAddress(145, 54);
    rack->setLugAddress(146, 59);
    rack->setLugAddress(147, 60);
    rack->setLugAddress(148, 65);
    rack->setLugAddress(149, 66);
    rack->setLugAddress(150, 71);
    rack->setLugAddress(151, 72);
    rack->setLugAddress(152, 77);
    rack->setLugAddress(153, 78);
    rack->setLugAddress(154, 83);
    rack->setLugAddress(155, 84);
    rack->setLugAddress(156, 89);
    rack->setLugAddress(157, 90);
    rack->setLugAddress(158, 95);
    rack->setLugAddress(159, 96);
    rack->setLugAddress(160, 101);
    rack->setLugAddress(161, 102);
    rack->setLugAddress(162, 107);
    rack->setLugAddress(163, 108);
    rack->setLugAddress(164, 113);
    rack->setLugAddress(165, 114);
    rack->setLugAddress(166, 119);
    rack->setLugAddress(167, 120);
    rack->setLugAddress(168, 125);
    rack->setLugAddress(169, 126);
    rack->setLugAddress(170, 131);
    rack->setLugAddress(171, 132);
    rack->setLugAddress(172, 137);
    rack->setLugAddress(173, 138);
    rack->setLugAddress(174, 143);
    rack->setLugAddress(175, 144);
    rack->setLugAddress(176, 149);
    rack->setLugAddress(177, 150);
    rack->setLugAddress(178, 155);
    rack->setLugAddress(179, 156);
    rack->setLugAddress(180, 161);
    rack->setLugAddress(181, 162);
    rack->setLugAddress(182, 167);
    rack->setLugAddress(183, 168);
    rack->setLugAddress(184, 173);
    rack->setLugAddress(185, 174);
    rack->setLugAddress(186, 179);
    rack->setLugAddress(187, 180);
    rack->setLugAddress(188, 185);
    rack->setLugAddress(189, 186);
    rack->setLugAddress(190, 191);
    rack->setLugAddress(191, 192);

    return rom;
}();

TEST_CASE("D192 Load")
{
    const QByteArrayView patch_rom(kPatchBin);

    SECTION("Read D192 ROM") {
        patchlib::Rom *actual = patchlib::Rom::create(patchlib::Rom::Type::D192);
        actual->loadFromData(patch_rom);
        REQUIRE(*kTestRom == *actual);
    }

    SECTION("Wrong ROM type") {
        patchlib::Rom *actual = patchlib::Rom::create(patchlib::Rom::Type::D192);

        const QByteArrayView badPatchRomView({0, 0, 0, 0, 0});
        REQUIRE_THROWS_AS(actual->loadFromData(badPatchRomView), patchlib::InvalidRomException);
    }

    SECTION("Bad lug") {
        auto bad_patch_rom = kPatchBin;
        bad_patch_rom[0x600] = 193; // Invalid lug.
        const QByteArrayView badPatchRomView(bad_patch_rom);

        patchlib::Rom *actual = patchlib::Rom::create(patchlib::Rom::Type::D192);
        REQUIRE_THROWS_AS(actual->loadFromData(badPatchRomView), patchlib::InvalidRomException);
    }
}

TEST_CASE("D192 Save")
{
    const auto actual = kTestRom->toByteArray();
    REQUIRE(kPatchBin.size() == actual.size());
    for (std::size_t ix = 0; ix < kPatchBin.size() && ix < actual.size(); ++ix) {
        if (static_cast<uint8_t>(kPatchBin[ix]) != static_cast<uint8_t>(actual[ix])) {
            FAIL("Mismatch at ix " << ix << " (Got " << static_cast<int>(actual[ix]) << " expected "
                                   << static_cast<unsigned int>(kPatchBin[ix]) << ")\n"
                                   << "Expected data:" << formatBytes(kPatchBin.cbegin(), kPatchBin.cend())
                                   << "Actual data:" << formatBytes(actual.cbegin(), actual.cend()));
        }
    }
}
