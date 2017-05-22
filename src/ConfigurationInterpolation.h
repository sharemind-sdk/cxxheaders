/*
 * Copyright (C) 2017 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#ifndef SHAREMIND_CONFIGURATIONINTERPOLATION_H
#define SHAREMIND_CONFIGURATIONINTERPOLATION_H

#include <boost/xpressive/xpressive_static.hpp>
#include <sstream>
#include <string>
#include <unordered_map>
#include "Exception.h"


namespace sharemind {

class ConfigurationInterpolation {

public: /* Types: */

    using Map = std::unordered_map<std::string, std::string>;

    SHAREMIND_DEFINE_EXCEPTION(sharemind::Exception, Exception);
    SHAREMIND_DEFINE_EXCEPTION_CONST_MSG(
            Exception,
            UnknownVariableException,
            "Unknown configuration interpolation variable!");

public: /* Methods: */

    ConfigurationInterpolation(Map const & m)
        : m_map(m)
    {}

    std::string operator()(std::string const & s) const {
        namespace xp = boost::xpressive;

        /* "([^%]|^)%\{([a-zA-Z\.]+)\}" */
        static xp::sregex const re =
            xp::imbue(std::locale("POSIX"))(
                (xp::bos | ~(xp::set= '%')) >>
                xp::as_xpr('%') >> '{' >>
                (xp::s1= + xp::set[ xp::range('a', 'z') |
                                    xp::range('A', 'Z') |
                                    '.' ]) >>
                '}');

        auto sIt(s.cbegin());
        std::stringstream ss;

        xp::sregex_iterator const reEnd;
        for (xp::sregex_iterator reIt(s.cbegin(), s.cend(), re);
             reIt != reEnd;
             (++reIt), (sIt = match[0].second))
        {
            auto const & match = *reIt;

            auto const it(m_map.find(match[1].str()));
            if (it != m_map.cend())
                throw UnknownVariableException();

            if (match[0].first != s.cbegin())
                ss.write(&*sIt, match[0].first + 1 - sIt);

            ss << it->second;
        }
        if (sIt != s.end())
            ss.write(&*sIt, s.end() - sIt);

        return ss.str();
    }

private: /* Fields: */

    Map const m_map;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_CONFIGURATIONINTERPOLATION_H */
