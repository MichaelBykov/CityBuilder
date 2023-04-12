/**
 * @file Markup.ipp
 * @brief Implementation of the markup utility.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "Markup.h"

NS_CITY_BUILDER_BEGIN

template<typename T>
template<typename U>
template<typename V>
typename Markup<T>::template Record<U> &
Markup<T>::template Record<U>::set(V U::*member, const List<V> &values) {
  typedef typename Markup<T>::_section::template _record<U> _record;
  _record *record = (_record *)markup._sections.setLast().records.last();
  
  if (values.count() != record->prefixes.count())
    // Mismatched count
    throw nullptr;
  
  record->set = [member, values](U &item, int index) {
    item.*member = values[index];
  };
  return *this;
}

template<typename T>
template<typename U>
typename Markup<T>::template Record<U> &
Markup<T>::template Record<U>::integer(int U::*member) {
  typedef typename Markup<T>::_section::template _record<U> _record;
  _record *record = (_record *)markup._sections.setLast().records.last();
  
  if (!record->matchers.isEmpty() &&
       record->matchers.last().type == _record::_matcher::Type::option) {
    // Add to the previous matcher
    record->matchers.setLast().option->matchers.append(member);
  } else {
    // Add to the end
    record->matchers.append(member);
  }
  
  return *this;
}

template<typename T>
template<typename U>
typename Markup<T>::template Record<U> &
Markup<T>::template Record<U>::real(Real U::*member) {
  typedef typename Markup<T>::_section::template _record<U> _record;
  _record *record = (_record *)markup._sections.setLast().records.last();
  
  if (!record->matchers.isEmpty() &&
       record->matchers.last().type == _record::_matcher::Type::option) {
    // Add to the previous matcher
    record->matchers.setLast().option->matchers.append(member);
  } else {
    // Add to the end
    record->matchers.append(member);
  }
  
  return *this;
}

template<typename T>
template<typename U>
typename Markup<T>::template Record<U> &
Markup<T>::template Record<U>::point(Real2 U::*member) {
  typedef typename Markup<T>::_section::template _record<U> _record;
  _record *record = (_record *)markup._sections.setLast().records.last();
  
  if (!record->matchers.isEmpty() &&
       record->matchers.last().type == _record::_matcher::Type::option) {
    // Add to the previous matcher
    record->matchers.setLast().option->matchers.append({
      member, _record::_matcher::Type::point
    });
  } else {
    // Add to the end
    record->matchers.append({
      member, _record::_matcher::Type::point
    });
  }
  
  return *this;
}

template<typename T>
template<typename U>
typename Markup<T>::template Record<U> &
Markup<T>::template Record<U>::vector(Real2 U::*member) {
  typedef typename Markup<T>::_section::template _record<U> _record;
  _record *record = (_record *)markup._sections.setLast().records.last();
  
  if (!record->matchers.isEmpty() &&
       record->matchers.last().type == _record::_matcher::Type::option) {
    // Add to the previous matcher
    record->matchers.setLast().option->matchers.append({
      member, _record::_matcher::Type::vector
    });
  } else {
    // Add to the end
    record->matchers.append({
      member, _record::_matcher::Type::vector
    });
  }
  
  return *this;
}

template<typename T>
template<typename U>
typename Markup<T>::template Record<U> &
Markup<T>::template Record<U>::identifier(const String &name) {
  typedef typename Markup<T>::_section::template _record<U> _record;
  _record *record = (_record *)markup._sections.setLast().records.last();
  
  if (!record->matchers.isEmpty() &&
       record->matchers.last().type == _record::_matcher::Type::option) {
    // Add to the previous matcher
    record->matchers.setLast().option->matchers.append(name);
  } else {
    // Add to the end
    record->matchers.append(name);
  }
  
  return *this;
}

template<typename T>
template<typename U>
typename Markup<T>::template Record<U>&
Markup<T>::template Record<U>::comma() {
  typedef typename Markup<T>::_section::template _record<U> _record;
  _record *record = (_record *)markup._sections.setLast().records.last();
  
  if (!record->matchers.isEmpty() &&
       record->matchers.last().type == _record::_matcher::Type::option) {
    // Add to the previous matcher
    record->matchers.setLast().option->matchers.append({ });
  } else {
    // Add to the end
    record->matchers.append({ });
  }
  
  return *this;
}

template<typename T>
template<typename U>
typename Markup<T>::template Record<U>&
Markup<T>::template Record<U>::option(const String &name) {
  typedef typename Markup<T>::_section::template _record<U> _record;
  _record *record = (_record *)markup._sections.setLast().records.last();
  
  record->matchers.append(new typename _record::_option(name));
  
  return *this;
}

template<typename T>
template<typename U>
template<typename V>
typename Markup<T>::template Record<U>&
Markup<T>::template Record<U>::match(V U::*member, const List<Pair<V>> &values) {
  typedef typename Markup<T>::_section::template _record<U> _record;
  _record *record = (_record *)markup._sections.setLast().records.last();
  
  if (!record->matchers.isEmpty() &&
       record->matchers.last().type == _record::_matcher::Type::option) {
    // Add to the previous matcher
    record->matchers.setLast().option->matchers.append(
      new typename _record::template _match<V>(member, values));
  } else {
    // Add to the end
    record->matchers.append(
      new typename _record::template _match<V>(member, values));
  }
  
  return *this;
}

template<typename T>
template<typename U>
Markup<T> &Markup<T>::template Record<U>::end() {
  Markup<T> &_markup = markup;
  delete this;
  return _markup;
}

template<typename T>
Markup<T> &Markup<T>::section(const String &name) {
  _sections.append(name);
  return *this;
}

template<typename T>
Markup<T> &Markup<T>::field(const String &name, String &value) {
  if (_sections.isEmpty())
    // No section
    throw nullptr;
  _sections.setLast().fields.append({ name, value });
  return *this;
}

template<typename T>
template<typename U>
typename Markup<T>::template Record<U> &
Markup<T>::records(const List<String> &names, List<U> &values) {
  if (_sections.isEmpty())
    // No section
    throw nullptr;
  _sections.setLast().records.append(new typename _section::template _record<U>(names, values));
  return *new typename Markup<T>::template Record<U>(this);
}

template<typename T>
template<typename U>
bool Markup<T>::_section::_record<U>::
parse(const List<Internal::Markup::Token> &tokens, T &item, int line, int selection) const {
  U value { };
  intptr_t index = 0;
  
  // Choose the setter
  if (set)
    set(value, selection);
  
  // Parse the content
  if (!_parse(tokens, matchers, index, value, line))
    return false;
  
  // Save the result
  values.append(value);
  return true;
}

template<typename T>
template<typename U>
bool Markup<T>::_section::_record<U>::
_parse(const List<Internal::Markup::Token> &tokens, List<_matcher> matchers, intptr_t &index, U &value, int line) const {
  auto error = [&](const String &message, int line, int column) {
    std::cout << "Error at line " << line << " col " << column << ": " << (const char *)message << std::endl;
  };
  
  auto unexpectedError = [&](const String &expected) {
    std::cout << "Error at line " << line << ": Unexpected end-of-line, expected" << (const char *)expected << ".\n";
  };
  
  // Go through the matchers
  for (const _matcher &matcher : matchers) {
    switch (matcher.type) {
    case _matcher::Type::integer: {
      // Check for a (positive) integer
      if (index >= tokens.count()) {
        unexpectedError("an integer");
        return false;
      }
      const Internal::Markup::Token &token = tokens[index];
      unsigned int integer;
      if (token.type != Internal::Markup::Token::Type::number || token.content.count('.') != 0) {
        error("Expected an integer.", token.line, token.column);
        return false;
      } else if (!token.content.tryParseUInt32(&integer) || (int)integer < 0) {
        error("Unable to parse an integer.", token.line, token.column);
        return false;
      } else {
        // Set the value
        value.*matcher.integer = integer;
      }
      index++;
    } break;
    
    case _matcher::Type::real: {
      // Check for a real number
      if (index >= tokens.count()) {
        unexpectedError("a real number");
        return false;
      }
      const Internal::Markup::Token &token = tokens[index];
      Real real;
      if (token.type != Internal::Markup::Token::Type::number) {
        error("Expected a real number.", token.line, token.column);
        return false;
      } else if (!token.content.tryParseReal(&real)) {
        error("Unable to parse a real number.", token.line, token.column);
        return false;
      } else {
        // Set the value
        value.*matcher.real = real;
      }
      index++;
    } break;
    
    case _matcher::Type::point: {
      // A point is a non-negative pair of numbers
      if (index + 2 >= tokens.count()) {
        unexpectedError("a point");
        return false;
      }
      const Internal::Markup::Token &xToken = tokens[index];
      const Internal::Markup::Token &comma  = tokens[index + 1];
      const Internal::Markup::Token &yToken = tokens[index + 2];
      Real x, y;
      if (xToken.type != Internal::Markup::Token::Type::number) {
        error("Expected a real number for the x-coordinate.", xToken.line, xToken.column);
        return false;
      } else if (!xToken.content.tryParseReal(&x)) {
        error("Unable to parse a real number.", xToken.line, xToken.column);
        return false;
      } else if (comma.type != Internal::Markup::Token::Type::comma) {
        error("Expected a comma.", comma.line, comma.column);
        return false;
      } else if (yToken.type != Internal::Markup::Token::Type::number) {
        error("Expected a real number for the y-coordinate.", yToken.line, yToken.column);
        return false;
      } else if (!yToken.content.tryParseReal(&y)) {
        error("Unable to parse a real number.", yToken.line, yToken.column);
        return false;
      } else {
        // Set the value
        value.*matcher.real2 = { x, y };
      }
      index += 3;
    } break;
    
    case _matcher::Type::vector: {
      // A vector is a pair of numbers with possible negative values
      if (index + 2 >= tokens.count()) {
        unexpectedError("a vector");
        return false;
      }
      
      Real xNegative = 1, yNegative = 1;
      Internal::Markup::Token xToken = tokens[index];
      if (xToken.type == Internal::Markup::Token::Type::identifier &&
          xToken.content == "-") {
        // Leading negative
        xNegative = -1;
        index++;
        xToken = tokens[index];
      }
      const Internal::Markup::Token &comma  = tokens[index + 1];
      Internal::Markup::Token yToken = tokens[index + 2];
      if (yToken.type == Internal::Markup::Token::Type::identifier &&
          yToken.content == "-") {
        // Leading negative
        yNegative = -1;
        index++;
        yToken = tokens[index];
      }
      
      Real x, y;
      if (xToken.type != Internal::Markup::Token::Type::number) {
        error("Expected a real number for the x-coordinate.", xToken.line, xToken.column);
        return false;
      } else if (!xToken.content.tryParseReal(&x)) {
        error("Unable to parse a real number.", xToken.line, xToken.column);
        return false;
      } else if (comma.type != Internal::Markup::Token::Type::comma) {
        error("Expected a comma.", comma.line, comma.column);
        return false;
      } else if (yToken.type != Internal::Markup::Token::Type::number) {
        error("Expected a real number for the y-coordinate.", yToken.line, yToken.column);
        return false;
      } else if (!yToken.content.tryParseReal(&y)) {
        error("Unable to parse a real number.", yToken.line, yToken.column);
        return false;
      } else {
        // Set the value
        value.*matcher.real2 = { xNegative * x, yNegative * y };
      }
      index += 3;
    } break;
    
    case _matcher::Type::identifier: {
      if (index >= tokens.count()) {
        unexpectedError("'" + matcher.identifier + "'");
        return false;
      }
      const Internal::Markup::Token &token = tokens[index];
      if (token.type != Internal::Markup::Token::Type::identifier ||
          token.content != matcher.identifier) {
        error("Expected '" + matcher.identifier + "'.", token.line, token.column);
        return false;
      }
      index++;
    } break;
    
    case _matcher::Type::comma: {
      if (index >= tokens.count()) {
        unexpectedError("','");
        return false;
      }
      const Internal::Markup::Token &token = tokens[index];
      if (token.type != Internal::Markup::Token::Type::comma) {
        error("Expected ','.", token.line, token.column);
        return false;
      }
      index++;
    } break;
    
    case _matcher::Type::match: {
      if (index >= tokens.count()) {
        unexpectedError("an identifier");
        return false;
      }
      const Internal::Markup::Token &token = tokens[index];
      if (token.type != Internal::Markup::Token::Type::identifier) {
        error("Expected an identifier.", token.line, token.column);
        return false;
      }
      if (!matcher.match->set(token.content, value)) {
        error("Unknown value '" + token.content + "'.", token.line, token.column);
        return false;
      }
      index++;
    } break;
    
    case _matcher::Type::option: {
      if (index >= tokens.count()) {
        // Not a required option
        continue;
      }
      const Internal::Markup::Token &token = tokens[index];
      if (token.type == Internal::Markup::Token::Type::identifier &&
          token.content == matcher.option->name) {
        // Match the options
        index++;
        if (!_parse(tokens, matcher.option->matchers, index, value, line))
          return false;
      }
    } break;
    }
  }
  return true;
}

template<typename T>
Markup<T>::operator bool() {
  // Tokenize the file
  List<Internal::Markup::Section> sections;
  if (!Internal::Markup::tokenizeMarkup(_path, sections)) {
    delete this;
    return false;
  }
  
  // Parse the data
  bool success = true;
  auto error = [&](const String &message, int line) {
    std::cout << "Error at line " << line << ": " << (const char *)message << std::endl;
    success = false;
  };
  for (const auto &section : sections)
    // Find the section
    for (const auto &_section : _sections)
      if (_section.name == section.name) {
        // Go through the entries
        for (const auto &entry : section.entries) {
          // Check if it's a field
          for (const auto &_field : _section.fields)
            if (_field.name == entry.identifier) {
              // Parse the field
              if (entry.tokens.count() != 1 ||
                  entry.tokens[0].type != Internal::Markup::Token::Type::string) {
                // A field can only be a string
                error("Expected a single string as input.", entry.line);
              } else {
                // Set the value
                _field.value = entry.tokens[0].content;
              }
              goto nextEntry;
            }
          
          // Check if it's a record
          for (const auto *_record : _section.records) {
            int index = 0;
            for (const String &_prefix : _record->prefixes)
              if (_prefix == entry.identifier) {
                // Match the record to the entry
                _record->parse(entry.tokens, _item, entry.line, index);
                goto nextEntry;
              } else
                index++;
          }
          
          // Nothing matched
          error("Unknown field '" + entry.identifier + "'.", entry.line);
          
          nextEntry:;
        }
        
        break;
      }
  
  delete this;
  return success;
}

template<typename T>
Markup<T> &parseMarkup(const String &path, T &item) {
  return *new Markup<T>(path, item);
}

NS_CITY_BUILDER_END
