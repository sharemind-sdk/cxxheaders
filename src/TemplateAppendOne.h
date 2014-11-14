/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_TEMPLATEAPPENDONE_H
#define SHAREMIND_TEMPLATEAPPENDONE_H


namespace sharemind {

template <template <typename ...> class Tmpl, typename, typename>
struct TemplateAppendOne;

template <template <typename ...> class Tmpl, typename T, typename ... Ts>
struct TemplateAppendOne<Tmpl, Tmpl<Ts...>, T> {
  using type = Tmpl<Ts..., T>;
};

} /* namespace Sharemind { */

#endif /* SHAREMIND_TEMPLATEAPPENDONE_H */
