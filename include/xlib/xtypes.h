/**
 * @file      xtypes.h
 * @brief     xlib types
 * @author    Vlad Sadovsky <vsadovsky at xevo.com>
 * @copyright Copyright (C) 2017 Xevo Inc. All Rights Reserved.
 *
 */
#pragma once

#ifndef XTYPES_H_
#define XTYPES_H_

/* missing types definition*/
#ifndef __cplusplus
#ifndef bool
typedef unsigned char bool;
#endif

#ifndef nullptr
#define nullptr (void *)NULL
#endif

#ifndef false
#define false   (unsigned char)0
#endif

#ifndef true
#define true    (unsigned char)1
#endif

#ifndef errno_t
typedef int errno_t;
#endif

#ifndef uint
typedef unsigned int uint;
#endif

#endif

#endif /* XTYPES_H_ */
