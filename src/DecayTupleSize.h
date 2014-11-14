/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_DECAYTUPLESIZE_H
#define SHAREMIND_DECAYTUPLESIZE_H

#include <tuple>


namespace sharemind {

template <typename T>
struct DecayTupleSize: ::std::tuple_size<typename ::std::decay<T>::type> {};

} /* namespace Sharemind { */

#endif /* SHAREMIND_DECAYTUPLESIZE_H */
