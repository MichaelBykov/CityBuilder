/**
 * @file Markup.h
 * @brief A utility for parsing custom markup files.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#pragma once
#include <CityBuilder/Common.h>
#include <CityBuilder/Storage/String.h>
#include <functional>
#include <iostream> // For errors

/*

HOW THE MARKUP PARSER WORKS
===========================



*/

NS_CITY_BUILDER_BEGIN
namespace Internal { namespace Markup {
  struct Token;
}}

template<typename T>
struct Markup;

template<typename T>
Markup<T> &parseMarkup(const String &path, T &item);

template<typename T>
struct Markup {
  template<typename U>
  struct Record {
    template<typename V>
    struct Pair {
      String name;
      V value;
    };
    
    template<typename V>
    Record &set(V U::*member, const List<V> &values);
    
    Record &integer(int U::*member);
    
    Record &real(Real U::*member);
    
    Record &point(Real2 U::*member);
    
    Record &vector(Real2 U::*member);
    
    Record &identifier(const String &value);
    
    Record &comma();
    
    Record &option(const String &name);
    
    template<typename V>
    Record &match(V U::*member, const List<Pair<V>> &values);
    
    Markup &end();
  
  private:
    Markup &markup;
    
    friend Markup;
    
    Record(Markup *markup) : markup(*markup) { }
  };
  
  Markup &section(const String &name);
  
  Markup &field(const String &name, String &value);
  
  template<typename U>
  Record<U> &records(const List<String> &names, List<U> &values);
  
  operator bool();
  
private:
  T &_item;
  String _path;
  
  Markup(const String &path, T &item) : _path(path), _item(item) { }
   
  friend Markup &::CityBuilder::parseMarkup(const String &path, T &item);
  
  struct _section {
    struct _field {
      String name;
      String &value;
    };
    
    String name;
    List<_field> fields { };
    
    struct _recordBase {
      List<String> prefixes;
      
      _recordBase(const List<String> &prefixes) : prefixes(prefixes) { }
      
      virtual ~_recordBase() { }
      
      virtual bool parse(const List<Internal::Markup::Token> &tokens, T &item, int line, int selection) const = 0;
    };
    
    template<typename U>
    struct _record : _recordBase {
      struct _option;
      
      struct _matchBase {
        virtual ~_matchBase() { }
        
        virtual _matchBase *clone() const = 0;
        
        virtual bool set(const String &value, U &item) = 0;
      };
      
      template<typename V>
      struct _match : _matchBase {
        List<typename Record<U>::template Pair<V>> values;
        V U::*member;
        
        _match(V U::*member, const List<typename Record<U>::template Pair<V>> &values) : member(member), values(values) { }
        
        _matchBase *clone() const override {
          return new _match(*this);
        }
        
        bool set(const String &value, U &item) override {
          for (const auto &pair : values) {
            if (pair.name == value) {
              item.*member = pair.value;
              return true;
            }
          }
          return false;
        }
      };
      
      
      struct _matcher {
        enum class Type {
          integer, real, point, vector, identifier, comma, option, match
        };
        
        union {
          int U::* integer;
          Real U::* real;
          Real2 U::* real2;
          String identifier;
          _option *option;
          _matchBase *match;
        };
        
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
      
      struct _option {
        String name;
        List<_matcher> matchers { };
        
        _option(const String &name) : name(name) { }
      };
      
      List<U> &values;
      
      std::function<void(U &item, int index)> set = nullptr;
      
      List<_matcher> matchers { };
      
      _record(const List<String> &names, List<U> &values) : _recordBase(names), values(values) { }
      
      bool parse(const List<Internal::Markup::Token> &tokens, T &item, int line, int selection) const override;
      
      bool _parse(const List<Internal::Markup::Token> &tokens, List<_matcher> matchers, intptr_t &index, U &value, int line) const;
    };
    
    List<_recordBase *> records { };
    
    _section(const String &name) : name(name) { }
    
    ~_section() {
      for (_recordBase *record : records)
        delete record;
    }
  };
  
  List<_section> _sections;
};



namespace Internal { namespace Markup {
  
  struct Token {
    enum class Type {
      section,
      identifier,
      number,
      comma,
      string,
      lineBreak,
    };
    String content;
    Type type;
    int line;
    int column;
  };
  
  struct Entry {
    String identifier;
    List<Token> tokens;
    int line;
  };
  
  struct Section {
    String name;
    List<Entry> entries;
    int line;
  };
  
  bool tokenizeMarkup(const String &path, List<Section> &sections);
  
}}
NS_CITY_BUILDER_END

#include "Markup.ipp"
