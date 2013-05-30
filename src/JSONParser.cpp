#include "JSONParser.h"
#include "DiskFile.h"
#include "OS.h"
#include "String.h"
#include <cassert>

namespace crown
{

//--------------------------------------------------------------------------
JSONParser::JSONParser(File* file, size_t size) :
	m_file(file),
	m_next_token(0),
	m_prev_token(-1),
	m_nodes_count(0)
{
	if (size > 1024)
	{
		m_tokens = new JSONToken[size];
	}
	else
	{
		m_tokens = m_tokens_list;
	}

	m_size = size;

	m_pos = m_file->position();

}

//--------------------------------------------------------------------------
JSONParser::~JSONParser()
{
	if (m_size > 1024 && m_tokens != NULL)
	{
		delete m_tokens;
	}
}

//--------------------------------------------------------------------------
JSONError JSONParser::parse()
{
	JSONError error;
	JSONToken* token;

	char c;

	while(!m_file->end_of_file())
	{
		JSONType type;

		m_file->read(&c, 1);
		m_pos = m_file->position();

		switch(c)
		{
			case '{':
			case '[':
			{
				token = allocate_token();

				if (token == NULL)
				{
					return JSON_NO_MEMORY;
				}
				if (m_prev_token != -1)
				{
					m_tokens[m_prev_token].m_size++;
					token->m_parent = m_prev_token;
				}

				token->m_type = c == '{' ? JSON_OBJECT : JSON_ARRAY;
				token->m_start = m_pos;
				m_prev_token = m_next_token - 1;

				break;
			}
			case '}':
			case ']':
			{
				type = c == '}' ? JSON_OBJECT : JSON_ARRAY;

				if (m_next_token < 1)
				{
					return JSON_INV_CHAR;
				}

				token = &m_tokens[m_next_token - 1];

				while (true)
				{
					if (token->m_start != -1 && token->m_end == -1)
					{
						if (token->m_type != type)
						{
							return JSON_INV_CHAR;
						}
						token->m_end = m_pos + 1;
						m_prev_token = token->m_parent;
						break;
					}

					if (token->m_parent == -1)
					{
						break;
					}

					token = &m_tokens[token->m_parent];
				}

				// token->m_size = token->m_end - token->m_start;
				fill_token(token, type, token->m_start, token->m_end);
				break;
			}
			case '\"':
			{
				error = parse_string();
            	if (m_prev_token != -1)
            	{
            		m_tokens[m_prev_token].m_size++;
            	}
				break;
			}
			case ':':
			{
				break;
			}
            case '\t': 
            case '\r': 
            case '\n': 
            case ',': 
            case ' ': 
            {
            	break;
            }
            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            case 't':
            case 'f':
            case 'n':
            {
            	error = parse_primitive();
            	if (m_prev_token != -1)
            	{
            		m_tokens[m_prev_token].m_size++;
            	}
            	break;
            }
		}
	}

	for (int i = m_next_token - 1; i >= 0; i--)
	{
		if (m_tokens[i].m_start != -1 && m_tokens[i].m_end == -1)
		{
			return JSON_INV_PART;
		}
	}

	return JSON_SUCCESS;
}

//--------------------------------------------------------------------------
JSONError JSONParser::parse_string()
{
	JSONToken* token;

	int start = m_pos;

	char c; 

	while(!m_file->end_of_file())
	{	
		m_file->read(&c, 1);
		m_pos = m_file->position();

		if (c == '\"' || c == '\'')
		{
			token = allocate_token();

			if (token == NULL)
			{
				m_pos = start;
				return JSON_NO_MEMORY;
			}

			fill_token(token, JSON_STRING, start + 1, m_pos);
			token->m_parent = m_prev_token;

			return JSON_SUCCESS;
		}

		if (c == '\\')
		{
			m_file->read(&c, 1);
			m_pos = m_file->position();

			switch(c)
			{
				case '\"': 
				case '/' : 
				case '\\': 
				case 'b':
                case 'f': 
                case 'r': 
                case 'n': 
                case 't':
                case 'u':
                {
                	break;
                }
                default:
               	{
                	m_pos = start;
                	return JSON_INV_CHAR;
                }
			}
		}
	}
	m_pos = start;
	return JSON_INV_PART;
}

//--------------------------------------------------------------------------
JSONError JSONParser::parse_primitive()
{
	JSONToken* token;

	int start = m_file->position();

	char c;

	while (!m_file->end_of_file())
	{
		m_file->read(&c, 1);
		m_pos = m_file->position();

		switch (c)
		{

			case ' ':
			case ',': 
			case '}':
			case ']':
			{
				token = allocate_token();

				if (token == NULL)
				{
					m_pos = start;
					return JSON_NO_MEMORY;
				}

				fill_token(token, JSON_PRIMITIVE, start, m_pos);

				token->m_parent = m_prev_token;

				m_file->seek(start);

				return JSON_SUCCESS;
			}
		}

		if (c < 32 || c >= 127)
		{
			m_pos = start;
			return JSON_INV_CHAR;
		}
	}
}

//--------------------------------------------------------------------------
JSONToken* JSONParser::allocate_token()
{
	JSONToken* token;

	if (m_next_token >= m_size)
	{
		return NULL;
	}	

	int32_t id = m_next_token++;

	token = &m_tokens[id];
	token->m_id = id;
	token->m_start = token->m_end = -1;
	token->m_size = 0;
	token->m_parent = -1;

	return token;
}

//--------------------------------------------------------------------------
void JSONParser::fill_token(JSONToken* token, JSONType type, int32_t start, int32_t end)
{
	uint32_t cur_pos = m_file->position();

	token->m_type = type;
	token->m_start = start;
	token->m_end = end;
	token->m_size = token->m_end - token->m_start;

	char tmp[token->m_size+1];
	m_file->seek(token->m_start-1);
	m_file->read(tmp, token->m_size);
	tmp[token->m_size] = '\0';
	string::strcpy(token->m_value, tmp);

	m_file->seek(cur_pos);
}

//--------------------------------------------------------------------------
JSONToken* JSONParser::get_tokens()
{
	return m_tokens_list;
}

//--------------------------------------------------------------------------
int32_t JSONParser::get_tokens_number()
{
	return m_next_token;
}

//--------------------------------------------------------------------------
JSONParser&	JSONParser::get_object(const char* key)
{
	int32_t begin = m_nodes_count != 0 ? m_nodes[m_nodes_count-1].m_id : 0;
	// For each token
	for (int i = begin; i < m_next_token; i++)
	{
		// Check key and type
		if ((string::strcmp(m_tokens[i].m_value, key) == 0)	&& m_tokens[i].m_type == JSON_STRING)
		{
			// Check if the successive token is an array
			assert(m_tokens[i+1].m_type == JSON_OBJECT);

			// Store token's id in a json node
			m_nodes[m_nodes_count].m_id = m_tokens[i+1].m_id;	
			m_nodes[m_nodes_count].m_type = JSON_OBJECT;
			m_nodes[m_nodes_count].print();

			// If token stored has parent
			if (m_tokens[i+1].has_parent())
			{
				// Check if precedent token stored is the parent of current token
				assert(m_nodes_count && m_nodes[m_nodes_count-1].m_id == m_tokens[i+1].m_parent);
			}

			break;
		}
	}
	// Incremente nodes count for the next token
	m_nodes_count++;

	return *this;
}

//--------------------------------------------------------------------------
JSONParser& JSONParser::get_array(const char* key)
{
	int32_t begin = m_nodes_count != 0 ? m_nodes[m_nodes_count-1].m_id : 0;

	// For each token
	for (int i = begin; i < m_next_token; i++)
	{
		// Check key and type
		if ((string::strcmp(m_tokens[i].m_value, key) == 0)	&& m_tokens[i].m_type == JSON_STRING)
		{
			// Check if the successive token is an array
			assert(m_tokens[i+1].m_type == JSON_ARRAY);

			// Store token's id in a json node
			m_nodes[m_nodes_count].m_id = m_tokens[i+1].m_id;	
			m_nodes[m_nodes_count].m_type = JSON_ARRAY;
			m_nodes[m_nodes_count].print();

			// If token stored has parent
			if (m_tokens[i+1].has_parent())
			{
				// Check if precedent token stored is the parent of current token
				assert(m_nodes_count && m_nodes[m_nodes_count-1].m_id == m_tokens[i+1].m_parent);
			}

			break;
		}
	}
	// Incremente nodes count for the next token
	m_nodes_count++;

	return *this;
}

//--------------------------------------------------------------------------
JSONParser&	JSONParser::get_string(const char* key)
{
	int32_t begin = m_nodes_count != 0 ? m_nodes[m_nodes_count-1].m_id : 0;

	// For each token
	for (int i = begin; i < m_next_token; i++)
	{
		// Check key and type
		if ((string::strcmp(m_tokens[i].m_value, key) == 0)	&& m_tokens[i].m_type == JSON_STRING)
		{
			// Check if the successive token is an array
			assert(m_tokens[i+1].m_type == JSON_STRING);

			// Store token's id in a json node
			m_nodes[m_nodes_count].m_id = m_tokens[i+1].m_id;	
			m_nodes[m_nodes_count].m_type = JSON_STRING;
			m_nodes[m_nodes_count].print();

			// If token stored has parent
			if (m_tokens[i+1].has_parent())
			{
				// Check if precedent token stored is the parent of current token
				assert(m_nodes_count && m_nodes[m_nodes_count-1].m_id == m_tokens[i+1].m_parent);
			}

			break;
		}
	}
	// Incremente nodes count for the next token
	m_nodes_count++;

	return *this;
}

//--------------------------------------------------------------------------
JSONParser& JSONParser::get_float(const char* key)
{

}

//--------------------------------------------------------------------------
JSONParser&	JSONParser::get_int(const char* key)
{

}

//--------------------------------------------------------------------------
JSONParser& JSONParser::get_bool(const char* key)
{

}

const char* JSONParser::to_string()
{
	return m_tokens[m_nodes[m_nodes_count-1].m_id].m_value;
}


} //namespace crown