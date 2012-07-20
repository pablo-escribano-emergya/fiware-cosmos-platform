
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_txt_parser_collocations_stripes
#define _H_SAMSON_txt_parser_collocations_stripes


#include <samson/module/samson.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include "samson/modules/txt/CountData.h"
#include "samson/modules/txt/Stripe.h"
#include <string>
#include <vector>



namespace samson{
namespace txt{


class parser_collocations_stripes : public samson::Parser
{
	samson::system::String key;
	samson::txt::Stripe value;
	int tam_ventana;

public:


	void init(samson::KVWriter *writer)
	{
		tam_ventana = environment->getInt( "txt.tam_ventana" , 3 );
	}

	void run( char *data , size_t length , samson::KVWriter *writer )
	{
		size_t pos_begin=0;
		size_t pos_end;

		std::string word;
		std::vector<std::string>entorno;

		//OLM_T(LMT_User06, ("Process data: '%s'", data));

		while (data[pos_begin] != '\0')
		{
			if (data[pos_begin] == '\266')
			{
				OLM_T(LMT_User06,("Filter parragraf at %d", pos_begin));
				data[pos_begin] = '\n';
			}
			pos_begin++;
		}
		pos_begin = 0;
		while (data[pos_begin] != '\0')
		{
			entorno.clear();
			//OLM_T(LMT_User06, ("Process at %d line: '%s'", pos_begin, data+pos_begin));
			while (true)
			{
				while((data[pos_begin]!='\0') && ((data[pos_begin]==' ') || (data[pos_begin]=='\r') || (data[pos_begin]=='\n')))
				{
					pos_begin++;
				}
				if ((data[pos_begin] == '\n') || (data[pos_begin] == '\0'))
				{
					//OLM_T(LMT_User06, ("Breaks after blanks at pos_begin:%d", pos_begin));
					break;
				}

				pos_end = pos_begin+1;

				while((data[pos_end]!='\0') && (data[pos_end]!=' ') && (data[pos_end]!='\r') && (data[pos_end]!='\n'))
				{
					pos_end++;
				}

				if((data[pos_end] == '\0') || (data[pos_end] == '\n'))
				{
					bool exitAfterWord = false;
					bool updatePosBegin = false;
					if (data[pos_end] == '\n')
					{
						data[pos_end] = '\0';
						exitAfterWord = true;
						updatePosBegin = true;
					}
					else if(data[pos_end] == '\0')
					{
						exitAfterWord = true;
					}

					if (data[pos_begin] != '\0')
					{
						word = std::string(&data[pos_begin]);
						entorno.push_back(word);
					}
					if (exitAfterWord)
					{
						if (updatePosBegin)
						{
							pos_begin = pos_end+1;
						}
						//OLM_T(LMT_User06, ("Breaks after word at pos_begin:%d", pos_begin));
						break;
					}
				}
				else
				{
					data[pos_end]='\0';
					if (data[pos_begin] != '\0')
					{
						entorno.push_back(&data[pos_begin]);
					}
				}

				pos_begin = pos_end+1;
			}

			std::vector<std::string>::iterator iter1 = entorno.begin();
			while (iter1 < entorno.end())
			{
				//value.clear();
				key = *iter1;

				//const char *cad1 = key.value.c_str();
				//OLM_T(LMT_User06, ("Works with '%s'", cad1));

				samson::txt::CountData item;
				std::string vacio = "__UNK__";
				item.word = vacio;
				item.count = 0;
				value.colListSetLength (2*tam_ventana);
				int i;
				for (i = 0; (i < (2*tam_ventana)); i++)
				{
					value.colList[i].copyFrom(&item);
				}

				std::vector<std::string>::iterator iter2 = iter1-1;

				for (i = 0; ((i < tam_ventana) && (iter2 >= entorno.begin())); i++, iter2--)
				{
					item.word = *iter2;
					item.count = 1;
					//OLM_T(LMT_User06,("Add bw '%s' en [%d]", item.word.value.c_str(), i));
					value.colList[i].copyFrom(&item);
				}
				//OLM_T(LMT_User06, ("Now let's look forward"));

				iter2 = iter1+1;
				for (int j = 0; ((j < tam_ventana) && (iter2 < entorno.end())); j++, i++, iter2++)
				{
					item.word = *iter2;
					item.count = 1;
					//OLM_T(LMT_User06,("Add fw '%s' en [%d]", item.word.value.c_str(), i));
					value.colList[i].copyFrom(&item);
				}


				//OLM_T(LMT_User06, ("Emit '%s'", cad1));
				writer->emit(0, &key, &value);
				iter1++;
			}
			if ((data[pos_begin] != '\0') && ((data[pos_begin] != '\n') || (data[pos_begin+1] != '\0')))
			{
				OLM_T(LMT_User06, ("Finished with line at data[%d]:'%c'", pos_begin, data[pos_begin]));
			}
			else
			{
				OLM_T(LMT_User06, ("Finished with data at data[%d]:'%c'", pos_begin, data[pos_begin]));
				return;
			}
		}
		return;
	}
};


} // end of namespace samson
} // end of namespace txt

#endif