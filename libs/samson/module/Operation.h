#ifndef SAMSON_OPERATION_H
#define SAMSON_OPERATION_H

#include <math.h>
#include <sstream>                 // std::ostringstream

#include <samson/module/KVSetStruct.h>    // KVSetStruct
#include <samson/module/KVWriter.h>       // KVWriter
#include <samson/module/KVFormat.h>       // KVFormat

#include <map>							// std::map

#include <samson/module/DataInstance.h>		// samson::Environment

#include <samson/module/Environment.h>		// samson::Environment
#include <samson/module/Tracer.h>              // samson::Tracer
#include <samson/module/OperationController.h>	// samson::OperationController

namespace samson {

	
	// Function used to "sort" when data type is used as key ( only in reduce operations )
	typedef int(* OperationInputCompareFunction)(KV *kv1 , KV*kv2);		
	
    
	/**
	 
	 \class samson::Operation
	 
	 Generic class for all custom operations.\n\n
	 Depending on the type of operation is desired you should subclass samson::Generator....\n
	 MRJobExtension is not intended to subclass directly.\n\n	 
	 
	 */
	
	class OperationInstance
	{
		
	public:
		
		Environment *environment;                   // Environment variables
		Tracer *tracer;                             // To send traces for debugging
		OperationController *operationController;	// Element to interact for operation stuff ( report progress at the moment )
		
        // Instances of the data types used for input and output
        std::vector< KVFormatDataInstances > inputData;
        std::vector< KVFormatDataInstances > outputData;
        
		OperationInstance()
		{
		}

		virtual ~OperationInstance()
		{
            for ( size_t i = 0 ; i  < inputData.size() ; i++ )
                inputData[i].destroy();
            for ( size_t i = 0 ; i  < outputData.size() ; i++ )
                outputData[i].destroy();
		}

		/** 
		 Optional method that can be implemented by user
		 */
		
		virtual void setup( std::string command ){}		
		
		/**
		 Optional method that can be implemented bu custom operation. It is suppoused to be called once at the begin of the operation.\n
		 Perfect place to alloc memory space if necessary
		 */
		
		virtual void init(){};		//Called once before running any operation
		
		/**
		 Optional method that can be implemented bu custom operation. It is suppoused to be called once at the end of the operation.\n
		 Perfect place to release memory space if necessary
		 */
		
		virtual void finish(){};	//Called once after all operations are executed

	};
	
	typedef void* (*CreationFunction)();

	class Operation
	{
		
	public:
		typedef enum  
		{ 
			generator,
			map,
			reduce,
			parser,
			parserOut,
			parserOutReduce,
			script,
			system,		// Spetial operation of the system
			unknown
		} Type;


		// Basic stuff
		std::string      _name;					//!< Name of the operation
		Type             _type;					//!< Identifier of the operation

		CreationFunction _creationFunction;		//!< Function to create new instances

		OperationInputCompareFunction _inputCompareFunction;			//!< Function used to sort inputs ( only reduce )
		OperationInputCompareFunction _inputCompareByKeyFunction;		//!< Function used to sort only by key ( to identify groups of key-values with the same key)
		
		std::vector<KVFormat> inputFormats;		//!< Formats of the key-value at the inputs
		std::vector<KVFormat> outputFormats;	//!< Format of the key-value at the outputs
		

		// Code in lines (scripts)
		std::vector<std::string> code;			//!< Code for scripts

		// Auxiliar stuff
		std::string _helpMessage;				//!< Help message shown on screen
		std::string _helpLine;					//!< Help in a line
        
		bool top;								//!< MACRO legacy parameter
		bool dynamic_input_formats;				//!< MACRO legacy parameter
		

		/**
		 Inform about the type of operation it is
		 */
		
		Operation(std::string name, Type type, CreationFunction creationFunction, OperationInputCompareFunction inputCompareFunction , OperationInputCompareFunction inputCompareByKeyFunction)
		{
			_type             = type;
			_creationFunction = creationFunction;
			_inputCompareFunction = inputCompareFunction;
			_inputCompareByKeyFunction = inputCompareByKeyFunction;

			top               = false;
			_name             = name;
			_helpLine         = "";
			_helpMessage      = "Help coming soon\n";
		}

		Operation(std::string name, Type type, CreationFunction creationFunction )
		{
			_type             = type;
			_creationFunction = creationFunction;
			_inputCompareFunction = NULL;
			_inputCompareByKeyFunction = NULL;
			
			top               = false;
			_name             = name;
			_helpLine         = "";
			_helpMessage      = "Help coming soon\n";
		}
		
		
		Operation( std::string name , Type type )
		{
			_type = type;
			_creationFunction = NULL;
			
			top = false;
			_name = name;
			_helpLine = "";
			_helpMessage = "Help coming soon\n";
		}
        
		Operation( Operation *op )
        {
			_type                           = op->_type;
			_creationFunction               = op->_creationFunction;
			_inputCompareFunction           = op->_inputCompareFunction;
			_inputCompareByKeyFunction      = op->_inputCompareByKeyFunction;
            
			top               = op->top;
            dynamic_input_formats= op->dynamic_input_formats;
            
			_name             = op->_name;
			_helpLine         = op->_helpLine;
			_helpMessage      = op->_helpMessage;
            
            // Copy the input formats
            inputFormats.insert( inputFormats.begin() , op->inputFormats.begin() , op->inputFormats.end() );
            outputFormats.insert( outputFormats.begin() , op->outputFormats.begin() , op->outputFormats.end() );
            
            // copy the code
            code.insert( code.begin() , op->code.begin() , op->code.end() );
        }
        
		virtual ~Operation()
		{
		}
        
        void getInfo( std::ostringstream& output)
        {
            output << "<operation>\n";
            output << "<name>" << _name << "</name>\n";
            output << "<type>" << getTypeName() << "</type>\n";
            
            output << "<input_description>" << inputFormatsString() << "</input_description>";
            output << "<output_description>" << outputFormatsString() << "</output_description>";
            
            output << "<input_formats>\n";
            for ( size_t i = 0 ; i < inputFormats.size() ; i++)
                inputFormats[i].getInfo( output );
            output << "</input_formats>\n";

            output << "<output_formats>\n";
            for ( size_t i = 0 ; i < outputFormats.size() ; i++)
                outputFormats[i].getInfo( output );
            output << "</output_formats>\n";
            
            output << "<help>" << _helpLine << "</help>\n";
            
            output << "</operation>\n";
        }
        

		void setHelpLine( std::string line )
		{
			_helpLine = line;
		}
		
		void setHelp( std::string help )
		{
			_helpMessage = help;
		}

		std::string help()
		{
			return _helpMessage;
		};									
		
		std::string helpLine()
		{
			return _helpLine;
		};									
		
		std::string getName()
		{
			return _name;
		}
		       
		void * getInstance(  )
		{
			return _creationFunction( );
		}
		
		
		int getNumInputs()
		{
			return inputFormats.size();
		}
		
		int getNumOutputs()
		{
			return outputFormats.size();
		}
		
		Type getType()
		{
			return _type;
		}
		
		std::string getTypeName()
		{
			switch (getType()) {
				case parser:            return "parser";	break;
				case parserOut:         return "parserOut"; break;
				case parserOutReduce:	return "parserReduce"; break;
				case map:               return "map";		break;
				case reduce:            return "reduce";	break;
				case generator:         return "generator"; break;
				case script:            return "script";	break;
				case system:            return "system";	break;
				case unknown:           return "unknown";	break;
			}
			return "?";
		}
		
		
		std::string inputFormatsString()
		{
			std::ostringstream o;
			for (int i = 0 ; i  < getNumInputs()  ; i++)
				o << "[" << inputFormats[i].str() << "]";
			return o.str();
		}
		
		std::string outputFormatsString()
		{
			std::ostringstream o;
			for (int i = 0 ; i  < getNumOutputs()  ; i++)
				o << "[" << outputFormats[i].str() << "]";
			return o.str();
		}
		
		std::string strHelp()
		{
			std::ostringstream o;
			
			o << "Help for operation " << _name <<  " ( " << getTypeName() << " )" <<  std::endl;
			o << "======================================================="<< std::endl;
			o << _helpLine << std::endl;
			o << "======================================================="<< std::endl;
			o << inputFormatsString() << " --> " << outputFormatsString() << std::endl; 
			o << "======================================================="<< std::endl;
			o << _helpMessage << std::endl;
			
			return o.str();
		}
		
		
		bool containsInputFormat( KVFormat f )
		{
			for (size_t i = 0 ; i < inputFormats.size() ;i++)
				if( inputFormats[i].isEqual( f ) )
					return true;
			
			return false;
		}
		
		bool containsOutputFormat( KVFormat f )
		{
			for (size_t i = 0 ; i < outputFormats.size() ;i++)
				if( outputFormats[i].isEqual( f ) )
					return true;
			
			return false;
		}
		
		
		std::vector<KVFormat> getInputFormats()
		{
			return inputFormats;
		}
		
		std::vector<KVFormat> getOutputFormats()
		{
			return outputFormats;
		}
	
		
		KVFormat getInputFormat( int i )
		{
			return inputFormats[i];
		}

		KVFormat getOutputFormat( int i )
		{
			return outputFormats[i];
		}
		
		OperationInputCompareFunction getInputCompareFunction()
		{
			return _inputCompareFunction;
		}

		OperationInputCompareFunction getInputCompareByKeyFunction()
		{
			return _inputCompareByKeyFunction;
		}
		
	};
	



	/**
	 
	 \class Generator
	 
	 A MRGenerator consist in creating a fileSet from scratch.
	 Only command line parameters introduced in the console can be used to generate the output. 
	 This class should be subclasses to create custom "generator".\n
	 This type of operations are very usefull to generate example fileSets to test map&reduce operations.\n
	 
	 */

	class Generator : public OperationInstance
	{
		
	public:
		/**
		 Main function to overload. Here your define the custom operation.\n
		 \param writer element to deliver output value pairs \n
		 */
		
		virtual void init(KVWriter *writer){};      //Called once before running any operation

        // Called once to inform the generator about how many different generators will run in paralel in the cluster
        virtual void setup( int worker , int num_workers, int process , int num_processes ){};
        
		virtual void run( KVWriter *writer )=0;
		
		virtual void finish(KVWriter *writer){};	//Called once after all operations are executed

	};
	
	/**
	 
	 \class Map
	 
	 A Map consist in creating one or more KVStorages from another one.
	 The map operation takes one KVSet at the input and process each key-value individually using the map functions
	 This class should be subclasses to create custom "mapper".\n
	 
	 */	
	
	class Map : public OperationInstance
	{
	public:
		/**
		 Main function to overload by the operation
		 */

		virtual void init(KVWriter *writer){};      //Called once before running any operation

		virtual void run(KVSetStruct* inputs , KVWriter *writer )=0;
		
		virtual void finish(KVWriter *writer){};	//Called once after all operations are executed

	};
	

	/**
	 
	 \class ParserOut
	 
	 A ParserOut consist in generating line-based txt files from KVs
	 This class should be subclasses to create custom "parserOut".
	 Operation is like a map but emitting txt instead of key-values
	 
	 */	
	
	class ParserOut : public OperationInstance
	{
	public:
		/**
		 Main function to overload by the operation
		 */

		virtual void init(TXTWriter *writer){};      //Called once before running any operation

		virtual void run(KVSetStruct* inputs , TXTWriter *writer )=0;

		virtual void finish(TXTWriter *writer){};	//Called once after all operations are executed

	};

	/**
	 
	 \class ParserOutReduce
	 
	 A ParserOut consist in generating line-based txt files from KVs
	 This class should be subclasses to create custom "parserOut".
	 Operation is like a map but emitting txt instead of key-values
	 
	 */	
	
	class ParserOutReduce : public OperationInstance
	{
	public:
		/**
		 Main function to overload by the operation
		 */

		virtual void init(TXTWriter *writer){};      //Called once before running any operation

		virtual void run(KVSetStruct* inputs , TXTWriter *writer )=0;

		virtual void finish(TXTWriter *writer){};	//Called once after all operations are executed
        
	};
	
    
	
	/**
	 
	 \class Reduce
	 
	 A Reduce consists in creating one or more KVStorages from a set of input KVStorages with the same type for the key.
	 The reduce operation takes multiple KVSets at the input ( one from each KVStorage) and process a set of key-values with the same key.
	 
	 This class should be subclasses to create custom "reducers".\n
	 
	 */
	
	class Reduce : public OperationInstance
	{
	public:
		/**
		 Main function to overload by the map
		 */

		virtual void init(KVWriter *writer){};      //Called once before running any operation

		virtual void run(KVSetStruct* inputs, samson::KVWriter *writer) = 0;
		
		virtual void finish(KVWriter *writer){};	//Called once after all operations are executed

	};
	
	
	/**
	 
	 \class Script
	 
	 A script is a program that combines multiple map / reduce / scripts to perform a task with multiple input / multiple output.
	 
	 This class should be subclassed to create custom "scripts".\n
	 
	 */
	
	class Script : public OperationInstance
	{
		// NO operation by itself ( only txt added in code field )
	};	
	
	
	/**
	 
	 \class Parser
	 
	 A parser is a generic operation that tranform input files (usualy txt files) into key-values
	 */
	
	class Parser : public OperationInstance
	{
	public:

		virtual void init(KVWriter *writer){};      //Called once before running any operation

		virtual void run( char *data , size_t length , samson::KVWriter *writer )=0;

		virtual void finish(KVWriter *writer){};	//Called once after all operations are executed
	};
	
}

#endif