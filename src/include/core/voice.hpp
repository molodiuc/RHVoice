/* Copyright (C) 2012  Olga Yakovleva <yakovleva.o.v@gmail.com> */

/* This program is free software: you can redistribute it and/or modify */
/* it under the terms of the GNU Lesser General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or */
/* (at your option) any later version. */

/* This program is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* GNU Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public License */
/* along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef RHVOICE_VOICE_HPP
#define RHVOICE_VOICE_HPP

#include <string>
#include <functional>
#include <set>
#include "RHVoice_common.h"
#include "smart_ptr.hpp"
#include "str.hpp"
#include "property.hpp"
#include "resource.hpp"
#include "params.hpp"
#include "language.hpp"
#include "sample_rate.hpp"
#include "hts_engine_pool.hpp"
#include "hts_engine_call.hpp"

namespace RHVoice
{
  class utterance;
  class client;
  class voice_info;

class voice
{
public:
  explicit voice(const voice_info& info);

  const voice_info& get_info() const
  {
    return info;
  }

  bool synthesize(const utterance& u,client& c) const;

private:
  voice(const voice&);
  voice& operator=(const voice&);

  const voice_info& info;
  mutable hts_engine_pool engine_pool;
};

  class voice_info: public resource_info<voice>
  {
  public:
    voice_info(const std::string& data_path,language_list& languages);

    language_list::const_iterator get_language() const
    {
      return voice_language;
    }

    sample_rate_t get_sample_rate() const
    {
      return sample_rate;
    }

    RHVoice_voice_gender get_gender() const
    {
      return gender;
    }

    std::string get_country() const
    {
      if(country.is_set())
        return country;
      else
        return voice_language->get_country();
    }

    voice_params settings;

    void register_settings(config& cfg);

    bool is_enabled()
    {
      return enabled&&voice_language->is_enabled();
    }

    bool is_preferred() const
    {
      return preferred;
    }

  private:
    smart_ptr<voice> create_instance() const
    {
      return smart_ptr<voice>(new voice(*this));
    }

    language_list::const_iterator voice_language;
    sample_rate_property sample_rate;
    enum_property<RHVoice_voice_gender> gender;
    bool_property enabled,preferred;
    string_property country;
  };

  class voice_list: public resource_list<voice_info>
  {
  public:
    voice_list(const std::vector<std::string>& voice_paths,language_list& languages);
  };

  class voice_search_criteria: public std::unary_function<const voice_info&,bool>
  {
  public:
    voice_search_criteria():
      preferred(false)
    {
    }

    void add_name(const std::string& name)
    {
      names.insert(name);
    }

    template<typename input_iterator>
    void add_names(input_iterator start,input_iterator end)
    {
      names.insert(start,end);
    }

    void clear_names()
    {
      names.clear();
    }

    void set_language(language_list::const_iterator lang)
    {
      voice_language=lang;
    }

    void clear_language()
    {
      voice_language=language_list::const_iterator();
    }

    void set_preferred()
    {
      preferred=true;
    }

    void clear_preferred()
    {
      preferred=false;
    }

    bool operator()(const voice_info& info) const;

    bool empty() const
    {
      return (names.empty()&&
              (voice_language==language_list::const_iterator())&&
              (!preferred));
    }

  private:
    std::set<std::string,str::less> names;
    language_list::const_iterator voice_language;
    bool preferred;
  };
}
#endif
