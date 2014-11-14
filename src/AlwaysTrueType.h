/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_ALWAYSTRUETYPE_H
#define SHAREMIND_ALWAYSTRUETYPE_H

#include <type_traits>


namespace sharemind {

template <typename ...>
using AlwaysTrueType = ::std::true_type;

} /* namespace Sharemind { */

#endif /* SHAREMIND_ALWAYSTRUETYPE_H */
