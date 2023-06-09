/**
 * @file Markup.cpp
 * @brief The tokenizer for the markup parser.
 * @date April 11, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Tools/Markup.h>
#include <CityBuilder/../../driver/Driver.h>
#include <iostream>

bool CityBuilder::Internal::Markup::tokenizeMarkup(const String &path, List<Section> &sections) {
  // First, read the file
  String file;
  {
    char *contents;
    size_t length;
    size_t index = 0, i = 0;
    for (wchar_t c : path) {
      if (c == '.')
        index = i;
      i++;
    }
    String extension = path.substring(index + 1, path.length());
    String _path = path.substring(0, index);
    if (!Driver::loadResource((const char *)_path, (const char *)extension, &contents, &length)) {
      std::cout << "Failed to load file '" << (const char *)path << "'." << std::endl;
      return false;
    }
    file = String(contents, length);
  }
  
  // Tokenize
  List<Token> tokens { };
  
  // Determine what token we're currently in
  enum class In {
    none,
    string,
    number,
    section,
    comment,
  };
  In in = In::none;
  
  // Status
  bool success = true;
  
  // Where we are in the file
  int line = 1, column = 1, startColumn = 1;
  
  // The buffer for the current token
  String buffer { };
  auto flush = [&]() {
    if (buffer.length() > 0) {
      tokens.append({ buffer, Token::Type::identifier, line, startColumn });
      buffer.removeAll();
    }
    startColumn = column;
  };
  
  // Standard error output
  auto error = [&](const String &message, int line, int column) {
    std::cout <<
      "Error in '" << (const char *)path << "' at line " << line <<
      " col " << column << ": " << (const char *)message << std::endl;
    success = false;
  };
  
  // Iterate through the file
  for (wchar_t c : file) {
    switch (in) {
    case In::none:
    none:
      if (c == '[') {
        // Start of a section
        flush();
        in = In::section;
        column++, startColumn++;
      } else if (c == '"') {
        // Start of a string
        flush();
        in = In::string;
        column++, startColumn++;
      } else if ('0' <= c && c <= '9') {
        // Start of a number
        flush();
        in = In::number;
        buffer.append(c);
        column++;
      } else if (c == ' ' || c == '\t') {
        // Whitespace
        flush();
        column++, startColumn++;
      } else if (c == '\n' || c == '\r') {
        // Line break
        flush();
        if (!tokens.isEmpty() && tokens.last().type != Token::Type::lineBreak)
          tokens.append({ c, Token::Type::lineBreak, line, column });
        line++;
        column = 1;
        startColumn = 1;
      } else if (c == ',') {
        // Comma
        flush();
        tokens.append({ L',', Token::Type::comma, line, column });
        buffer.removeAll();
        column++, startColumn++;
      } else if (c == '#') {
        // Comment
        flush();
        in = In::comment;
        column++;
      } else {
        // Identifier
        buffer.append(c);
        column++;
      }
      break;
    
    case In::string:
      // Handle strings
      if (c == '"') {
        tokens.append({ buffer, Token::Type::string, line, startColumn });
        buffer.removeAll();
        in = In::none;
        column++;
        startColumn = column;
      } else {
        buffer.append(c);
        if (c == '\n' || c == '\r') {
          line++;
          column = 1;
        } else {
          column++;
        }
      }
      break;
      
    case In::number:
      // Handle numbers
      if ('0' <= c && c <= '9' || c == '.' && buffer.count('.') == 0) {
        buffer.append(c);
      } else {
        // Start of another token: flush and handle
        tokens.append({ buffer, Token::Type::number, line, startColumn });
        buffer.removeAll();
        in = In::none;
        column++;
        startColumn = column;
        goto none;
      }
      break;
    
    case In::section:
      // Handle sections
      if (c == ']') {
        tokens.append({ buffer, Token::Type::section, line, startColumn });
        buffer.removeAll();
        in = In::none;
        column++;
        startColumn = column;
      } else {
        if (c == '\n' || c == '\r')
          error("Unexpected end-of-line in section name.", line, column);
        buffer.append(c);
        column++;
      }
      break;
    
    case In::comment:
      // Handle comments
      if (c == '\n' || c == '\r') {
        // End of line: done
        in = In::none;
        if (tokens.last().type != Token::Type::lineBreak)
          tokens.append({ c, Token::Type::lineBreak, line, column });
        line++;
        column = 1;
        startColumn = 1;
      } else {
        column++;
      }
      break;
    }
  }
  // Check that we finished up our tokens
  switch (in) {
  case In::string:
    error("Unexpected end-of-file in string.", line, column);
    return false;
  
  case In::section:
    error("Unexpected end-of-file in section name.", line, column);
    return false;
  
  case In::number:
    // Finish the number
    tokens.append({ buffer, Token::Type::number, line, startColumn });
    buffer.removeAll();
    break;
  
  case In::none:
    flush();
  case In::comment:
    break;
  }
  // Final line break, as needed
  if (tokens.isEmpty()) {
    std::cout << "No content found in the file '" << (const char *)path << "'." << std::endl;
    return false;
  } else if (tokens.last().type != Token::Type::lineBreak)
    tokens.append({ L'\n', Token::Type::lineBreak, line, column });
  
  // Don't cause errors-on-top-of-errors
  if (!success)
    return false;
  
  // Cleanup
  sections.removeAll();
  for (intptr_t i = 0; i < tokens.count(); i++) {
    const Token &token = tokens[i];
    if (tokens[i].type == Token::Type::section) {
      // Create the new section
      sections.append({ token.content, { }, token.line });
    } else if (sections.isEmpty()) {
      error("Invalid data without a section.", token.line, token.column);
    } else if (token.type == Token::Type::lineBreak) {
      // Ignore
    } else {
      // Check for a leading identifier
      if (token.type != Token::Type::identifier)
        error("Invalid data without an identifier.", token.line, token.column);
      
      // Go to through the rest of the line
      Entry entry { token.content, { }, token.line };
      i++;
      while (i < tokens.count() && tokens[i].type != Token::Type::lineBreak)
        entry.tokens.append(tokens[i++]);
      
      // Save the entry
      sections.setLast().entries.append(entry);
    }
  }
  
  return success;
}
