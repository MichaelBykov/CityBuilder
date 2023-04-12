/**
 * @file ConfigurationParser.cpp
 * @brief Implementation of the configuration parser utility.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Building/Road.h>
#include <iostream>
USING_NS_CITY_BUILDER

namespace {
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
  };
  
  struct Entry {
    String identifier;
    List<Token> tokens;
  };
}

bool RoadDef::load(const String &path, RoadDef &road) {
  // First, read the file
  bool success;
  String file = String::readFile((const char *)path, &success);
  if (!success) {
    std::cout << "Failed to load road file: " << (const char *)path << std::endl;
    return false;
  }
  
  // Tokenize
  List<Token> tokens { };
  enum class In {
    none,
    string,
    number,
    section,
    comment,
  };
  In in = In::none;
  String buffer { };
  auto flush = [&]() {
    if (buffer.length() > 0) {
      tokens.append({ buffer, Token::Type::identifier });
      buffer.removeAll();
    }
  };
  for (wchar_t c : file) {
    switch (in) {
    case In::none:
    none:
      if (c == '[') {
        flush();
        in = In::section;
      } else if (c == '"') {
        flush();
        in = In::string;
      } else if ('0' <= c && c <= '9') {
        flush();
        in = In::number;
        buffer.append(c);
      } else if (c == ' ' || c == '\t') {
        // Whitespace
        flush();
      } else if (c == '\n' || c == '\r') {
        flush();
        if (!tokens.isEmpty() && tokens.last().type != Token::Type::lineBreak)
          tokens.append({ c, Token::Type::lineBreak });
      } else if (c == ',') {
        flush();
        tokens.append({ L',', Token::Type::comma });
        buffer.removeAll();
      } else if (c == '#') {
        // Comment
        flush();
        in = In::comment;
      } else {
        buffer.append(c);
      }
      break;
    
    case In::string:
      // Handle strings
      if (c == '"') {
        tokens.append({ buffer, Token::Type::string });
        buffer.removeAll();
        in = In::none;
      } else
        buffer.append(c);
      break;
      
    case In::number:
      // Handle numbers
      if ('0' <= c && c <= '9' || c == '.' && buffer.count('.') == 0) {
        buffer.append(c);
      } else {
        // Start of another token: flush and handle
        tokens.append({ buffer, Token::Type::number });
        buffer.removeAll();
        in = In::none;
        goto none;
      }
      break;
    
    case In::section:
      // Handle sections
      if (c == ']') {
        tokens.append({ buffer, Token::Type::section });
        buffer.removeAll();
        in = In::none;
      } else
        buffer.append(c);
      break;
    
    case In::comment:
      // Handle comments
      if (c == '\n' || c == '\r') {
        in = In::none;
        if (tokens.last().type != Token::Type::lineBreak)
          tokens.append({ c, Token::Type::lineBreak });
      }
      break;
    }
  }
  switch (in) {
  case In::string:
  case In::section:
    std::cout << "Unfinished token at the end of the file." << std::endl;
    return false;
  
  case In::number:
    // Finish the number
  
  case In::none:
    flush();
  case In::comment:
    break;
  }
  if (tokens.isEmpty()) {
    std::cout << "No content found in the file '" << (const char *)path << "'." << std::endl;
    return false;
  } else if (tokens.last().type != Token::Type::lineBreak)
    tokens.append({ L'\n', Token::Type::lineBreak });
  
  // Cleanup
  enum class Section : int {
    none = -1,
    road = 0,
    profile,
    traffic,
    dividers,
    texture
  };
  List<Entry> entries[5] { { } };
  Section section = Section::none;
  for (intptr_t i = 0; i < tokens.count(); i++) {
    const Token &token = tokens[i];
    if (tokens[i].type == Token::Type::section) {
      // Check which section is selected
      const String &text = token.content;
      if (text == "road")
        section = Section::road;
      else if (text == "profile")
        section = Section::profile;
      else if (text == "traffic")
        section = Section::traffic;
      else if (text == "dividers")
        section = Section::dividers;
      else if (text == "texture")
        section = Section::texture;
      else {
        std::cout << "Unknown section: " << (const char *)text << std::endl;
        return false;
      }
    } else if (section == Section::none) {
      std::cout << "Invalid data without a section." << std::endl;
      return false;
    } else {
      // Check for a leading identifier
      if (token.type != Token::Type::identifier) {
        std::cout << "Invalid data without an identifier." << std::endl;
        return false;
      }
      
      // Go to through the rest of the line
      Entry entry { token.content, { } };
      while (i < tokens.count() && tokens[i].type != Token::Type::lineBreak)
        entry.tokens.append(tokens[i++]);
      
      // Save the entry
      entries[(int)section].append(entry);
    }
  }
  
  // Go through each entry
  { // Road
    // There should be exactly one entry: road name
    const List<Entry> &fields = entries[(int)Section::road];
    if (fields.count() != 1 || fields[0].identifier != "name") {
      std::cout << "A [road] section can only have one entry, name." << std::endl;
      return false;
    }
    
    if (fields[0].tokens.count() != 1 || fields[0].tokens[0].type != Token::Type::string) {
      std::cout << "The name entry in a [road] section must be a string." << std::endl;
      return false;
    }
    
    // Set the name
    road.name = fields[0].tokens[0].content;
  }
  { // Profile
    // A list of points
    for (const Entry &entry : entries[(int)Section::profile]) {
      Point point { };
      
      // Check the identifier
      if (entry.identifier != "M" && entry.identifier != "D" && entry.identifier != "C") {
        std::cout << "The [profile] section can only have point entries." << std::endl;
        return false;
      }
      point.type =
        entry.identifier == "M" ? Point::Type::move :
        entry.identifier == "D" ? Point::Type::disjoint :
        Point::Type::connected;
      
      // Must start with a point
      if (entry.tokens.count() < 3 ||
          entry.tokens[0].type != Token::Type::number ||
          entry.tokens[1].type != Token::Type::comma  ||
          entry.tokens[2].type != Token::Type::number) {
        std::cout << "An entry in the [profile] section must start with a number pair." << std::endl;
      }
      
      // Extract the point
      if (!entry.tokens[0].content.tryParseReal(&point.position.x) ||
          !entry.tokens[2].content.tryParseReal(&point.position.y)) {
        std::cout << "Unable to read a point pair in [profile]." << std::endl;
        return false;
      }
      
      // Check what else to do
      int uv = 0, normal = 0;
      for (intptr_t i = 3; i < entry.tokens.count(); i++) {
        // Each subentry must start with an identifier
        if (entry.tokens[i].type != Token::Type::identifier) {
          std::cout << "A subentry in the [profile] section must start with an identifier." << std::endl;
          return false;
        }
        
        if (entry.tokens[i].content == "uv") {
          // Add a uv coordinate
        } else if (entry.tokens[i].content == "normal") {
          // Add a normal
        } else {
          std::cout << "Unknown identifier in the [profile] section: " << (const char *)entry.tokens[i].content << std::endl;
          return false;
        }
      }
    }
  }
  
  return true;
}
