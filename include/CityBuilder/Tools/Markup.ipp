/**
 * @file Markup.ipp
 * @brief Implementation of the markup utility.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include "Markup.h"
#include <functional>
#include <iostream> // For errors

NS_CITY_BUILDER_BEGIN

/* -------------------------------------------------------------------------- *\
|                                                                              |
| Tokenization                                                                 |
|                                                                              |
\* -------------------------------------------------------------------------- */

namespace Internal { namespace Markup {
  
  /// A parsed file token.
  struct Token {
    /// The type of token.
    enum class Type {
      /// A section header, e.x. `[foo]`.
      section, 
      /// An identifier (default type).
      identifier,
      /// A number (including decimals).
      number,
      /// A single comma.
      comma,
      /// A string.
      string,
      /// A line break.
      lineBreak,
    };
    
    /// The text content of the token.
    String content;
    
    /// The token content type.
    Type type;
    
    /// The line number of the token.
    int line;
    
    /// The column number of the token.
    int column;
  };
  
  /// A parsed line entry.
  /// \remarks
  ///   An entry is any line under a section that follows the following pattern:
  ///   ```
  ///   name [value1] [value2] [value3] ...
  ///   ```
  ///   Note that an entry is exactly one line long.
  struct Entry {
    /// The name of the entry.
    String identifier;
    /// The tokens belonging to the entry.
    List<Token> tokens;
    /// The line number of the entry.
    int line;
  };
  
  /// A parsed section.
  struct Section {
    /// The name of the section.
    String name;
    /// The entries under to the section.
    List<Entry> entries;
    /// The line number that the section begins at, including the section
    /// header.
    int line;
  };
  
  /// Tokenize a custom markup file.
  /// \param[in] path
  ///   The path to the markup file.
  /// \param[out] sections
  ///   The parsed sections of the markup file.
  bool tokenizeMarkup(const String &path, List<Section> &sections);
  
}}





/* -------------------------------------------------------------------------- *\
|                                                                              |
| Markup internal section interface                                            |
|                                                                              |
\* -------------------------------------------------------------------------- */

/// The internal markup section layout.
template<typename T>
struct Markup<T>::_section {
  /// A field in the section.
  struct _field {
    /// The name of the field.
    String name;
    /// The place to assign the parsed value to.
    String &value;
  };
  
  
  
  
  
  /// The base type of a record definition, for type erasure.
  struct _recordBase {
    /// The prefixes that the record can be identified by.
    List<String> prefixes;
    
    _recordBase(const List<String> &prefixes) : prefixes(prefixes) { }
    
    virtual ~_recordBase() { }
    
    /// Parse a record.
    /// \param[in] tokens
    ///   The tokens to parse.
    /// \param[out] item
    ///   The item to parse into.
    /// \param[in] line
    ///   The line number of the record.
    /// \param[in] selection
    ///   The selection index of the record (from the prefixes).
    /// \returns
    ///   Whether or not the record was parsed successfully.
    virtual bool parse(
      const List<Internal::Markup::Token> &tokens,
      T &item,
      int line,
      int selection
    ) const = 0;
  };
  
  
  
  
  
  
  /// A concrete record definition.
  template<typename U>
  struct _record : _recordBase {
    struct _option;
    
    /// The base type of a match definition, for type erasure.
    struct _matchBase {
      virtual ~_matchBase() { }
      
      /// Clone the match definition.
      virtual _matchBase *clone() const = 0;
      
      /// Set the value of the match.
      /// \param[in] value
      ///   The string value to match for setting.
      /// \param[out] item
      ///   The item on which to set.
      /// \returns
      ///   Whether or not the value was set.
      virtual bool set(const String &value, U &item) = 0;
    };
    
    
    
    /// A concrete match definition.
    template<typename V>
    struct _match : _matchBase {
      /// An association of named values to match.
      List<typename Record<U>::template Named<V>> values;
      
      /// The member to set.
      V U::*member;
      
      _match(
        V U::*member,
        const List<typename Record<U>::template Named<V>> &values
      ) : member(member), values(values) { }
      
      /// Clone the match definition.
      _matchBase *clone() const override {
        return new _match(*this);
      }
      
      // Set the actual match value
      bool set(const String &value, U &item) override {
        for (const auto &named : values) {
          if (named.name == value) {
            item.*member = named.value;
            return true;
          }
        }
        return false;
      }
    };
    
    
    
    /// A parameter matcher definition.
    struct _matcher {
      /// The type of the matcher.
      enum class Type {
        /// A positive integer type.
        integer,
        /// A positive floating-point type.
        real,
        /// A positive point type.
        point,
        /// A signed vector type.
        vector,
        /// An identifier.
        identifier,
        /// A comma.
        comma,
        /// An option set.
        option,
        /// A value match (think switch statement for an identifier).
        match
      };
      
      union {
        /// The integer member to set.
        int U::* integer;
        /// The real member to set.
        Real U::* real;
        /// The 2-dimensional real member to set.
        Real2 U::* real2;
        /// The identifier to match.
        String identifier;
        /// The option set to match.
        _option *option;
        /// The value match set.
        _matchBase *match;
      };
      
      /// The type of the matcher.
      Type type;
      
      _matcher(int U::* integer) : integer(integer), type(Type::integer) { }
      _matcher(Real U::* real) : real(real), type(Type::real) { }
      _matcher(Real2 U::* real2, Type type) : real2(real2), type(type) { }
      _matcher(const String &identifier) : identifier(identifier), type(Type::identifier) { }
      _matcher(_option *option) : option(option), type(Type::option) { }
      _matcher(_matchBase *match) : match(match), type(Type::match) { }
      _matcher() : type(Type::comma) { }
      
      _matcher(const _matcher &other) : type(other.type) {
        switch (type) {
        case Type::integer: integer = other.integer; break;
        case Type::real: real = other.real; break;
        case Type::point: real2 = other.real2; break;
        case Type::vector: real2 = other.real2; break;
        case Type::identifier: new (&identifier) String(other.identifier); break;
        case Type::option: option = new _option(*other.option); break;
        case Type::match: match = other.match->clone(); break;
        case Type::comma: break;
        }
      }
      
      _matcher(_matcher &&other) : type(other.type) {
        switch (type) {
        case Type::integer: integer = other.integer; break;
        case Type::real: real = other.real; break;
        case Type::point: real2 = other.real2; break;
        case Type::vector: real2 = other.real2; break;
        case Type::identifier: new (&identifier) String(other.identifier); break;
        case Type::option: option = other.option; break;
        case Type::comma: match = other.match; break;
        case Type::match: break;
        }
        other.type = Type::comma;
      }
      
      ~_matcher() {
        switch (type) {
        case Type::identifier: identifier.~String(); break;
        case Type::option: delete option; break;
        case Type::match: delete match; break;
        default: break;
        }
      }
    };
    
    
    
    /// An option set definition.
    struct _option {
      /// The name of the option set.
      String name;
      
      /// The matchers for the option set arguments.
      List<_matcher> matchers { };
      
      _option(const String &name) : name(name) { }
    };
    
    
    
    /// The list to append parsed values to.
    List<U> &values;
    
    /// An optional setter for turning the parsed name of the record into a
    /// value.
    std::function<void(U &item, int index)> set = nullptr;
    
    /// The matchers for the record.
    List<_matcher> matchers { };
    
    _record(const List<String> &names, List<U> &values)
      : _recordBase(names), values(values) { }
    
    // Implementation of the parse method
    bool parse(
      const List<Internal::Markup::Token> &tokens,
      T &item,
      int line,
      int selection
    ) const override;
    
    /// A parser for a token stream.
    /// \param[in] tokens
    ///   The token stream to parse.
    /// \param[in] matchers
    ///   The matchers to use for parsing.
    /// \param[inout] index
    ///   The first index of the token to parse.
    /// \param[out] value
    ///   The actual record value to parse into.
    /// \param[in] line
    ///   The line number of the token stream.
    /// \returns
    ///   Whether or not the token stream was successfully parsed.
    bool _parse(
      const List<Internal::Markup::Token> &tokens,
      List<_matcher> matchers,
      intptr_t &index,
      U &value,
      int line
    ) const;
  };
  
  
  
  
  
  /// The name of the section.
  String name;
  
  /// The fields of the section.
  List<_field> fields { };
  
  /// The records of the section.
  List<_recordBase *> records { };
  
  _section(const String &name) : name(name) { }
  
  ~_section() {
    for (_recordBase *record : records)
      delete record;
  }
};





/* -------------------------------------------------------------------------- *\
|                                                                              |
| Markup record builder                                                        |
|                                                                              |
\* -------------------------------------------------------------------------- */

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
  
  if (record->set)
    // Can't have multiple setters
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
Markup<T>::template Record<U>::match(V U::*member, const List<Named<V>> &values) {
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





/* -------------------------------------------------------------------------- *\
|                                                                              |
| Markup section builder                                                       |
|                                                                              |
\* -------------------------------------------------------------------------- */

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
  _sections.setLast().records.append(
    new typename _section::template _record<U>(names, values));
  return *new typename Markup<T>::template Record<U>(this);
}





/* -------------------------------------------------------------------------- *\
|                                                                              |
| Markup parsing                                                               |
|                                                                              |
\* -------------------------------------------------------------------------- */

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
  
  if (index < tokens.count()) {
    // Not all tokens were consumed
    std::cout << "Error at line " << line << " col " << tokens[index].column << ": Unused value.\n";
    return false;
  }
  
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
      // Match the exact identifier
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
      // Match a single comma
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
      // Match one of the provided identifiers
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
