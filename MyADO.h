// MyADO.h: interface for the CMyADO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYADO_H__E144D98C_2388_4800_BC00_F7E963816A73__INCLUDED_)
#define AFX_MYADO_H__E144D98C_2388_4800_BC00_F7E963816A73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#import <msado15.dll> no_namespace rename( "EOF", "adoEOF" )
//#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF", "EndOfFile")


#import "C:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename( "EOF", "adoEOF" )




class CMyADO  
{
public:
	
	
	CMyADO();
	virtual ~CMyADO();

	HRESULT Open( _bstr_t btConnectionString, _bstr_t btUserID, _bstr_t btPassword );
	HRESULT Close();

	HRESULT AddParameterReturnValue();
//	HRESULT AddParameterInputLongLong( _bstr_t btParameterName, __int64 lValue );
	HRESULT AddParameterInputLong( _bstr_t btParameterName, long lValue );
	HRESULT AddParameterInputText( _bstr_t btParameterName, _bstr_t btValue );

	HRESULT AddParameterInputOutputLong( _bstr_t btParameterName, long lValue );
	HRESULT AddParameterInputOutputText( _bstr_t btParameterName, _bstr_t btValue, DWORD dwMaxTextSize );

	HRESULT AddParameterOutputLong( _bstr_t btParameterName );
	HRESULT AddParameterOutputText( _bstr_t btParameterName, DWORD dwMaxTextSize );

	HRESULT Execute();

	HRESULT GetFieldLong( _bstr_t btFieldName, long* plValue );
	HRESULT GetFieldText( _bstr_t btFieldName, char* szText, DWORD dwMaxTextSize );

	HRESULT GetFieldTextByIndex( int nColumnIndex, char* pName , DWORD dwMaxNameSize , 
									char* pValue , DWORD dwMaxValueSize );
	

	HRESULT GetParameterReturnValue( long* plReturnValue );

	HRESULT GetParameterLong( _bstr_t btParameterName, long* plValue );
	HRESULT GetParameterText( _bstr_t btParameterName, char* szText, DWORD dwMaxTextSize );

	HRESULT Initialize( _bstr_t btStoredProcedureName );

	BOOL IsEOF();

	HRESULT MoveNext();
	HRESULT Move( long nPos , long nStartType = 0 );

	LPSTR GetErrorStr();
	int IsErrorStr();

	HRESULT GetRecordCount( long* lRecordCount );
	HRESULT GetFieldCount( long* lFieldCount );


	void Lock() { m_bLock = true ; m_LockTime = GetTickCount(); }
	void UnLock() { m_bLock = false ; m_LockTime = 0; }
	bool IsLcok() { return m_bLock; }

	bool m_bLock;

	bool IsFree() 
	{ 
		if( IsLcok() )		// 잠겨 있는 경우 	
			return false;
		else				// 잠겨 있지 않는 경우
		{		
			//if( 0 == m_nUseCount )
			//	return false;
			//else
				return true;
		}			
	}

	int m_nUseCount;

	DWORD m_LockTime;
	void EndUsing();

	HRESULT GetValue( char* pstr , _variant_t vtValue , int dwMaxTextSize );
	

protected:

	//HRESULT GetRecordCount( long* lRecordCount );

private:
	
	
	void ResetError();
	void SetComError( _com_error& eComError );
	void SetGerError();
	HRESULT AddParameter( _bstr_t btParameterName, DataTypeEnum enDataType, ParameterDirectionEnum enParameterDirection, long lSize, _variant_t vtValue );
	HRESULT GetField( _variant_t vtFieldName, _variant_t& vtValue );
	HRESULT GetParameter( _variant_t vtParameterName, _variant_t& vtValue );

	HRESULT GetFieldByColumnIndex( int nColumnIndex , _variant_t& vtName , _variant_t& vtValue );



	BOOL IsConnected();
	BOOL IsInitialized();

	_ConnectionPtr m_pConnectionPtr;
	_CommandPtr m_pCommandPtr;
	_RecordsetPtr m_pRecordsetPtr;

	char m_szLastErrorMsg[1024];

};

#ifndef MY_ADO_MACRO
#define MY_ADO_MACRO

#define ADO_FUNC_ERR_MSG( obj , FuncName , exit_line )		\
if( obj.IsErrorStr() )										\
	::MessageBox( NULL , obj.GetErrorStr() , "[CMyADO::" #FuncName "] error" , MB_OK );	\
exit_line;


#define ADO_INIT( obj , sp_name , exit_line )   \
if( obj.Initialize( sp_name ) != S_OK )		\
{												\
	ADO_FUNC_ERR_MSG( obj , Initialize , exit_line )	\
}

#define ADO_EXEC( obj , exit_line )					\
if( obj.Execute() != S_OK )							\
{													\
	ADO_FUNC_ERR_MSG( obj , Execute , exit_line )	\
}


#define ADO_ADD_RETURN( obj , exit_line )	\
if( obj.AddParameterReturnValue() != S_OK )	\
{											\
	ADO_FUNC_ERR_MSG( obj , AddParameterReturnValue , exit_line ) \
}

#define ADO_ADD_INPUT_LONGLONG( obj , field_text , value_longlong , exit_line )		\
if( obj.AddParameterInputLong( field_text , (__int64)value_longlong ) != S_OK )	\
{																			\
	ADO_FUNC_ERR_MSG( obj , AddParameterInputLong , exit_line )				\
}


#define ADO_ADD_INPUT_LONG( obj , field_text , value_long , exit_line )		\
if( obj.AddParameterInputLong( field_text , (int)value_long ) != S_OK )	\
{																			\
	ADO_FUNC_ERR_MSG( obj , AddParameterInputLong , exit_line )				\
}

#define ADO_ADD_INPUT_TEXT( obj , field_text , value_text , exit_line )	\
if( obj.AddParameterInputText( field_text , value_text ) != S_OK )	\
{																		\
	ADO_FUNC_ERR_MSG( obj , AddParameterInputText , exit_line )			\
}

#define ADO_ADD_OUTPUT_LONG( obj , field_text , exit_line )		 \
if( obj.AddParameterOutputLong( field_text ) != S_OK )			 \
{																 \
	ADO_FUNC_ERR_MSG( obj , AddParameterOutputLong , exit_line ) \
}

#define ADO_ADD_OUTPUT_TEXT( obj , field_text , field_size , exit_line )	\
if( obj.AddParameterOutputText( field_text , field_size ) != S_OK )		\
{																		\
	ADO_FUNC_ERR_MSG( obj , AddParameterOutputText , exit_line )		\
}



#define ADO_ADD_INOUT_TEXT( obj , field_text , value_text , field_size , exit_line )	\
if( obj.AddParameterInputOutputText( field_text , value_text , field_size ) != S_OK )	\
{																						\
	ADO_FUNC_ERR_MSG( obj , AddParameterInputOutputText , exit_line )					\
}

#define ADO_ADD_INOUT_LONG( obj , field_text , value_text , exit_line )			\
if( obj.AddParameterInputOutputLong( field_text , value_text ) != S_OK )		\
{																				\
	ADO_FUNC_ERR_MSG( obj , AddParameterInputOutputLong , exit_line )			\
}




#define ADO_GET_RET( obj , ret_value , exit_line )			      \
if( obj.GetParameterReturnValue( ret_value ) != S_OK )			  \
{																  \
	ADO_FUNC_ERR_MSG( obj , GetParameterReturnValue , exit_line ) \
}


#define ADO_GET_LONG( obj , field_text , ret_value , exit_line )	\
if( obj.GetParameterLong( field_text , ret_value ) != S_OK )			\
{																		\
	ADO_FUNC_ERR_MSG( obj , GetParameterLong , exit_line )				\
}

#define ADO_GET_TEXT( obj , field_text , ret_value , ret_size , exit_line )	\
if( obj.GetParameterText( field_text , ret_value , ret_size ) != S_OK )	\
{																		\
	ADO_FUNC_ERR_MSG( obj , GetParameterText , exit_line )				\
}

///////////////////////////////////////////////////
#define PADO_FUNC_ERR_MSG( obj , FuncName , exit_line )		\
if( obj->IsErrorStr() )										\
	::MessageBox( NULL , obj->GetErrorStr() , "[CMyADO::" #FuncName "] error" , MB_OK );	\
exit_line;


#define PADO_INIT( obj , sp_name , exit_line )   \
if( obj->Initialize( sp_name ) != S_OK )		\
{												\
	PADO_FUNC_ERR_MSG( obj , Initialize , exit_line )	\
}

#define PADO_EXEC( obj , exit_line )					\
if( obj->Execute() != S_OK )							\
{													\
	PADO_FUNC_ERR_MSG( obj , Execute , exit_line )	\
}


#define PADO_ADD_RETURN( obj , exit_line )	\
if( obj->AddParameterReturnValue() != S_OK )	\
{											\
	PADO_FUNC_ERR_MSG( obj , AddParameterReturnValue , exit_line ) \
}


#define PADO_ADD_INPUT_LONG( obj , field_text , value_long , exit_line )		\
if( obj->AddParameterInputLong( field_text , (int)value_long ) != S_OK )	\
{																			\
	PADO_FUNC_ERR_MSG( obj , AddParameterInputLong , exit_line )				\
}

#define PADO_ADD_INPUT_TEXT( obj , field_text , value_text , exit_line )	\
if( obj->AddParameterInputText( field_text , value_text ) != S_OK )	\
{																		\
	PADO_FUNC_ERR_MSG( obj , AddParameterInputText , exit_line )			\
}

#define PADO_ADD_OUTPUT_LONG( obj , field_text , exit_line )		 \
if( obj->AddParameterOutputLong( field_text ) != S_OK )			 \
{																 \
	PADO_FUNC_ERR_MSG( obj , AddParameterOutputLong , exit_line ) \
}

#define PADO_ADD_OUTPUT_TEXT( obj , field_text , field_size , exit_line )	\
if( obj->AddParameterOutputText( field_text , field_size ) != S_OK )		\
{																		\
	PADO_FUNC_ERR_MSG( obj , AddParameterOutputText , exit_line )		\
}



#define PADO_ADD_INOUT_TEXT( obj , field_text , value_text , field_size , exit_line )	\
if( obj->AddParameterInputOutputText( field_text , value_text , field_size ) != S_OK )	\
{																						\
	PADO_FUNC_ERR_MSG( obj , AddParameterInputOutputText , exit_line )					\
}

#define PADO_ADD_INOUT_LONG( obj , field_text , value_text , exit_line )			\
if( obj->AddParameterInputOutputLong( field_text , value_text ) != S_OK )		\
{																				\
	PADO_FUNC_ERR_MSG( obj , AddParameterInputOutputLong , exit_line )			\
}




#define PADO_GET_RET( obj , ret_value , exit_line )			      \
if( obj->GetParameterReturnValue( ret_value ) != S_OK )			  \
{																  \
	PADO_FUNC_ERR_MSG( obj , GetParameterReturnValue , exit_line ) \
}


#define PADO_GET_LONG( obj , field_text , ret_value , exit_line )	\
if( obj->GetParameterLong( field_text , ret_value ) != S_OK )			\
{																		\
	PADO_FUNC_ERR_MSG( obj , GetParameterLong , exit_line )				\
}

#define PADO_GET_TEXT( obj , field_text , ret_value , ret_size , exit_line )	\
if( obj->GetParameterText( field_text , ret_value , ret_size ) != S_OK )	\
{																		\
	PADO_FUNC_ERR_MSG( obj , GetParameterText , exit_line )				\
}

///////////////////////////////////////////////////

#endif


#endif // !defined(AFX_MYADO_H__E144D98C_2388_4800_BC00_F7E963816A73__INCLUDED_)
