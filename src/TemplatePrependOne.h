/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_TEMPLATEPREPENDONE_H
#define SHAREMIND_TEMPLATEPREPENDONE_H


namespace sharemind {

template <template <typename ...> class Tmpl, typename, typename>
struct TemplatePrependOne;

template <template <typename ...> class Tmpl, typename T, typename ... Ts>
struct TemplatePrependOne<Tmpl, Tmpl<Ts...>, T> {
  using type = Tmpl<T, Ts...>;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATEPREPENDONE_H */
