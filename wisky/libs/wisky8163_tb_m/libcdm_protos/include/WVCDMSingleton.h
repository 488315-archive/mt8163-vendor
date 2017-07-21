//
// Copyright 2013 Google Inc. All Rights Reserved.
//

#ifndef WV_CDM_SINGLETON_H_
#define WV_CDM_SINGLETON_H_

#include "utils/StrongPointer.h"

#include "wv_content_decryption_module.h"

namespace wvdrm {

android::sp<wvcdm::WvContentDecryptionModule> getCDM();

} // namespace wvdrm

#endif // WV_CDM_SINGLETON_H_
