/* 
 * Copyright 2012 Xamarin Inc
 */
#ifndef __MONO_METADATA_CLASS_INTERBALS_H__
#define __MONO_METADATA_CLASS_INTERBALS_H__

#include "class.h"

MONO_BEGIN_DECLS

MONO_API MonoClass *
mono_class_from_name_checked (MonoImage *image, const char* name_space, const char *name, MonoError *error);

MONO_API MonoClass *
mono_class_from_name_case_checked (MonoImage *image, const char* name_space, const char *name, MonoError *error);

MONO_END_DECLS

#endif /* __MONO_METADATA_CLASS_INTERBALS_H__ */
