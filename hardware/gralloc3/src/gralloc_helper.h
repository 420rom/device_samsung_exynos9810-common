/*
 * Copyright (C) 2010-2019 ARM Limited. All rights reserved.
 *
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GRALLOC_HELPER_H_
#define GRALLOC_HELPER_H_

#include <unistd.h>
#include <sys/mman.h>
#include <android/log.h>

#include "VendorVideoAPI.h"

#ifndef AWAR
#define AWAR(fmt, args...) \
	__android_log_print(ANDROID_LOG_WARN, "[Gralloc-Warning]", "%s:%d " fmt, __func__, __LINE__, ##args)
#endif
#ifndef AINF
#define AINF(fmt, args...) __android_log_print(ANDROID_LOG_INFO, "[Gralloc]", fmt, ##args)
#endif
#ifndef AERR
#define AERR(fmt, args...) \
	__android_log_print(ANDROID_LOG_ERROR, "[Gralloc-ERROR]", "%s:%d " fmt, __func__, __LINE__, ##args)
#endif
#ifndef AERR_IF
#define AERR_IF(eq, fmt, args...) \
	if ((eq))                     \
	AERR(fmt, args)
#endif

#define GRALLOC_ALIGN(value, base) ((((value) + (base) -1) / (base)) * (base))

#define GRALLOC_MAX(a, b) (((a)>(b))?(a):(b))

#define GRALLOC_UNUSED(x) ((void)x)

/* LSI integration */
#define GRALLOC1_USAGE_SW_WRITE_MASK                          0x000000F0
#define GRALLOC1_USAGE_SW_READ_MASK                           0x0000000F
#define GRALLOC1_USAGE_READ_OFTEN                            ((1ULL << 1) | (1ULL << 2))
#define GRALLOC1_USAGE_WRITE_OFTEN                           ((1ULL << 5) | (1ULL << 6))

#define VIDEO_PRIV_DATA_SIZE                                  sizeof(ExynosVideoMeta)

#define PLANE_SIZE(w, h, ext_size)                            ((w) * (h) + ext_size)
// 10bit format
#define P010_PLANE_SIZE(w, h, ext_size)                       ((((w) * 2) * (h)) + ext_size)

// 8+2 format
#define NV12N_S8B_LUMA_SIZE(w, h, ext_size)                   PLANE_SIZE(w, h, ext_size)
#define NV12N_S8B_CHROMA_SIZE(w, h, ext_size)                 (GRALLOC_ALIGN(((w) * (h)) + ext_size, 16))
#define NV12N_S2B_SIZE(w, h)                                  ((GRALLOC_ALIGN((w) / 4, 16) * (h)) + 64)
#define NV12M_S8B_SIZE(w, h, ext_size)                        PLANE_SIZE(w, h, ext_size)

#ifdef GRALLOC_MSCL_ALIGN_RESTRICTION
#define NV12M_S2B_LUMA_SIZE(w, h, ext_size)                   ((GRALLOC_ALIGN((w) / 4, 16) * (GRALLOC_ALIGN(h, 16))) + ext_size)
#define NV12M_S2B_CHROMA_SIZE(w, h, ext_size)                 ((GRALLOC_ALIGN((w) / 4, 16) * (GRALLOC_ALIGN(h, 16) / 2)) + ext_size)
#else
#define NV12M_S2B_LUMA_SIZE(w, h, ext_size)                   ((GRALLOC_ALIGN((w) / 4, 16) * (h)) + ext_size)
#define NV12M_S2B_CHROMA_SIZE(w, h, ext_size)                 ((GRALLOC_ALIGN((w) / 4, 16) * (h / 2)) + ext_size)
#endif

/* helper macros */
#ifndef __ALIGN_UP
#define __ALIGN_UP(x, a)					(((x) + ((a) - 1)) & ~((a) - 1))
#endif

/* SBWC align macros */
#define SBWC_8B_STRIDE(w)					(128 * (((w) + 31) / 32))
#define SBWC_10B_STRIDE(w)					(160 * (((w) + 31) / 32))
#define SBWC_HEADER_STRIDE(w)				((((((w) + 63) / 64) + 15) / 16) * 16)

#define SBWC_Y_VSTRIDE_BLOCKS(h)			((__ALIGN_UP((h), 16) + 3) / 4)
#define SBWC_CBCR_VSTRIDE_BLOCKS(h)			(((__ALIGN_UP((h), 16) / 2) + 3) / 4)

#define SBWC_8B_Y_SIZE(w, h)				((SBWC_8B_STRIDE(w) * ((__ALIGN_UP((h), 16) + 3) / 4)) + 64)
#define SBWC_8B_Y_HEADER_SIZE(w, h)			__ALIGN_UP(((SBWC_HEADER_STRIDE(w) * ((__ALIGN_UP((h), 16) + 3) / 4)) + 256), 32)
#define SBWC_8B_CBCR_SIZE(w, h)				((SBWC_8B_STRIDE(w) * (((__ALIGN_UP((h), 16) / 2) + 3) / 4)) + 64)
#define SBWC_8B_CBCR_HEADER_SIZE(w, h)		((SBWC_HEADER_STRIDE(w) * (((__ALIGN_UP((h), 16) / 2) + 3) / 4)) + 128)

#define SBWC_10B_Y_SIZE(w, h)				((SBWC_10B_STRIDE(w) * ((__ALIGN_UP((h), 16) + 3) / 4)) + 64)
#define SBWC_10B_Y_HEADER_SIZE(w, h)		__ALIGN_UP((((__ALIGN_UP((w), 32) * __ALIGN_UP((h), 16) * 2) + 256) - SBWC_10B_Y_SIZE(w, h)), 32)
#define SBWC_10B_CBCR_SIZE(w, h)			((SBWC_10B_STRIDE(w) * (((__ALIGN_UP((h), 16) / 2) + 3) / 4)) + 64)
#define SBWC_10B_CBCR_HEADER_SIZE(w, h)		(((__ALIGN_UP((w), 32) * __ALIGN_UP((h), 16)) + 256) - SBWC_10B_CBCR_SIZE(w, h))

/* SBWC Lossy align macros */
#define SBWCL_8B_STRIDE(w, r)				(((128 * (r)) / 100) * (((w) + 31) / 32))
#define SBWCL_10B_STRIDE(w, r)				(((160 * (r)) / 100) * (((w) + 31) / 32))

#define SBWCL_8B_Y_SIZE(w, h, r)			((SBWCL_8B_STRIDE(w, r) * ((__ALIGN_UP((h), 8) + 3) / 4)) + 64)
#define SBWCL_8B_CBCR_SIZE(w, h, r)			((SBWCL_8B_STRIDE(w, r) * (((__ALIGN_UP((h), 8) / 2) + 3) / 4)) + 64)

#define SBWCL_10B_Y_SIZE(w, h, r)			((SBWCL_10B_STRIDE(w, r) * ((__ALIGN_UP((h), 8) + 3) / 4)) + 64)
#define SBWCL_10B_CBCR_SIZE(w, h, r)		((SBWCL_10B_STRIDE(w, r) * (((__ALIGN_UP((h), 8) / 2) + 3) / 4)) + 64)

#define SBWCL_8B_CBCR_BASE(base, w, h, r)	((base) + SBWCL_8B_Y_SIZE(w, h, r))
#define SBWCL_10B_CBCR_BASE(base, w, h, r)	((base) + SBWCL_10B_Y_SIZE(w, h, r))

// MSCL
#define MSCL_ALIGN                                            128

#ifdef GRALLOC_MSCL_ALIGN_RESTRICTION
#define MSCL_EXT_SIZE                                         512
#else
#define MSCL_EXT_SIZE                                         0
#endif

#ifdef GRALLOC_MSCL_ALLOC_RESTRICTION
#define SIZE_4K                                               4096
#else
#define SIZE_4K                                               0
#endif

static inline size_t round_up_to_page_size(size_t x)
{
	return (x + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);
}

#endif /* GRALLOC_HELPER_H_ */
