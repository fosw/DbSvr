// MyADO.cpp: implementation of the CMyADO class.
//
// Copyright 2003 Nathan Davies
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "MyADO.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//	Class Constructor
CMyADO::CMyADO()
{
	m_pCommandPtr = NULL;
	
	ResetError();
	m_nUseCount = 0;
	m_bLock = false;

	m_LockTime = 0;
}


void CMyADO::EndUsing()
{
	m_nUseCount = 0;

	UnLock();
}


//	Class Destructor
CMyADO::~CMyADO()
{
	if( m_pRecordsetPtr )
	{
		if( m_pRecordsetPtr->State == adStateOpen )
			m_pRecordsetPtr->Close();

		m_pRecordsetPtr = NULL;
	}

	m_pCommandPtr = NULL;
}

//	Create a Parameter and Add it to the CommandPtr Object (Which will be used to Execute the Stored Procedure)
HRESULT CMyADO::AddParameter( _bstr_t btParameterName, DataTypeEnum enDataType, ParameterDirectionEnum enParameterDirection, long lSize, _variant_t vtValue )
{
	HRESULT hReturn = S_FALSE;

	ResetError();

	if( IsConnected() && IsInitialized() )
	{
		try
		{
			_ParameterPtr pParameterPtr = m_pCommandPtr->CreateParameter( btParameterName, enDataType, enParameterDirection, lSize, vtValue );
			m_pCommandPtr->Parameters->Append( pParameterPtr );

			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{			
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();
		}
	}

	return hReturn;
}

//	Add Parameter Heler Function for Long Type and Input Direction
//HRESULT CMyADO::AddParameterInputLongLong( _bstr_t btParameterName, __int64 lValue )
//{
//	return AddParameter( btParameterName, adBigInt, adParamInput, sizeof( __int64 ), _variant_t( lValue ));
//}


//	Add Parameter Heler Function for Long Type and Input Direction
HRESULT CMyADO::AddParameterInputLong( _bstr_t btParameterName, long lValue )
{
	return AddParameter( btParameterName, adInteger, adParamInput, sizeof( long ), _variant_t( lValue ));
}

//	Add Parameter Helper Function for Text Type and Input Direction
HRESULT CMyADO::AddParameterInputText( _bstr_t btParameterName, _bstr_t btValue )
{
	return AddParameter( btParameterName, adBSTR, adParamInput, btValue.length(), _variant_t( btValue ));
}

//	Add Parameter Helper Function for Long Type and Input/Output Direction
HRESULT CMyADO::AddParameterInputOutputLong( _bstr_t btParameterName, long lValue )
{
	return AddParameter( btParameterName, adInteger, adParamInputOutput, sizeof( long ), _variant_t( lValue ));
}

//	Add Parameter Helper Function for Text Type and Input/Output Direction
HRESULT CMyADO::AddParameterInputOutputText( _bstr_t btParameterName, _bstr_t btValue, DWORD dwMaxTextSize )
{
	return AddParameter( btParameterName, adBSTR, adParamInputOutput, dwMaxTextSize, _variant_t( btValue ));
}

//	Add Parameter Helper Function for Long Type and Output Direction
HRESULT CMyADO::AddParameterOutputLong( _bstr_t btParameterName )
{
	_variant_t vtNull;

	return AddParameter( btParameterName, adInteger, adParamOutput, 0, vtNull );
}

//	Add Parameter Helper Function for Text Type and Output Direction
HRESULT CMyADO::AddParameterOutputText( _bstr_t btParameterName, DWORD dwMaxTextSize )
{
	_variant_t vtNull;

	return AddParameter( btParameterName, adVarChar, adParamOutput, dwMaxTextSize, vtNull );
}

//	Add Parameter Helper Function for Return Value
HRESULT CMyADO::AddParameterReturnValue()
{
	_variant_t vtNull;

	return AddParameter( "RETURN_VALUE", adInteger, adParamReturnValue, 0, vtNull );
}

//	Close the Current ADO Connection
HRESULT CMyADO::Close()
{
	HRESULT hReturn = S_FALSE;

	ResetError();

	if( m_pConnectionPtr )
	{
		if( m_pConnectionPtr->State == adStateOpen )
		{
			try
			{
				hReturn = m_pConnectionPtr->Close();

				m_pConnectionPtr = NULL;
			}
			catch( _com_error& eComError )
			{
				SetComError( eComError );
			}
			catch( ... )
			{
				SetGerError();
			}
		}
		else
			hReturn = S_OK;
	}
	else
		hReturn = S_OK;

	return hReturn;
}
//	Execute the Stored Procedure using the CommandPtr Object
HRESULT CMyADO::Execute()
{
	HRESULT hReturn = S_FALSE;
	
	ResetError();

	if( IsConnected() && IsInitialized() )
	{
		try
		{
			m_pRecordsetPtr = m_pCommandPtr->Execute( NULL, NULL, adCmdStoredProc );

			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{			
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();	
		}
	}

	return hReturn;
}

//	Retrieve a Value from the Recordset (which was created during Stored Procedure Execution)
HRESULT CMyADO::GetField( _variant_t vtFieldName, _variant_t& vtValue )
{
	HRESULT hReturn = S_FALSE;
	
	ResetError();

	if( IsConnected() && IsInitialized() )
	{
		try
		{
			vtValue = m_pRecordsetPtr->Fields->GetItem( vtFieldName )->Value;			

			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();
		}
	}

	return hReturn;
}


HRESULT CMyADO::GetValue( char* pstr , _variant_t vtValue , int dwMaxTextSize )
{
	HRESULT hReturn = NULL;

	switch( vtValue.vt )
		{
		case VT_BSTR:
		case VT_DATE:
			{
				_bstr_t btValue = ( _bstr_t )vtValue;

				if( dwMaxTextSize < btValue.length())
					hReturn = S_FALSE;
				else
					strcpy( pstr, btValue );
			}
			break;
		case VT_I2:
			sprintf( pstr, "%ld", vtValue.iVal );
			break;
		case VT_I4:
			sprintf( pstr, "%ld", vtValue.lVal );
			break;
		case VT_R4:
			sprintf( pstr, "%lf", vtValue.fltVal );			
			break;
		case VT_R8:
			sprintf( pstr, "%lf", vtValue.dblVal );			
			break;
		case VT_DECIMAL:
			if( 0 == vtValue.decVal.sign )
				sprintf( pstr, "%I64d", vtValue.decVal.Lo64 );			
			else
				sprintf( pstr, "-%I64d", vtValue.decVal.Lo64 );			
			break;
		case VT_BOOL:
			if( 0 == vtValue.boolVal )
				strcpy( pstr, "0" );		
			else
				strcpy( pstr, "1" );		
			break;
		case VT_UI1:
			sprintf( pstr, "%ld", vtValue.iVal );
			break;
		case VT_EMPTY:
		case VT_NULL:
			strcpy( pstr, "" );	
			break;
		default:
			strcpy( pstr, "" );	
			
			break;
		}	
	
	return hReturn;
}

HRESULT CMyADO::GetFieldTextByIndex( int nColumnIndex, char* pName , DWORD dwMaxNameSize , 
									char* pValue , DWORD dwMaxValueSize )
{
	_variant_t vtName;
	_variant_t vtValue;

	HRESULT hReturn = GetFieldByColumnIndex( nColumnIndex , vtName , vtValue );

	if( hReturn == S_OK )
	{
		_bstr_t btName = ( _bstr_t )vtName;	
		if( dwMaxNameSize < btName.length())
			hReturn = S_FALSE;
		else
			strcpy( pName, btName );

		hReturn = GetValue( pValue , vtValue , dwMaxValueSize );
	
	}

	return hReturn;
}

HRESULT CMyADO::GetFieldByColumnIndex( int nColumnIndex , _variant_t& vtName , _variant_t& vtValue )
{
	HRESULT hReturn = S_FALSE;
	
	ResetError();

	if( IsConnected() && IsInitialized() )
	{
		try
		{	
			_variant_t vrIndex = (LONG)nColumnIndex;
			vtName = m_pRecordsetPtr->Fields->GetItem( vrIndex )->GetName();
			vtValue = m_pRecordsetPtr->Fields->GetItem( vrIndex )->GetValue();

			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();
		}
	}

	return hReturn;
}


//	Get Field Helper Function for Long Type
HRESULT CMyADO::GetFieldLong( _bstr_t btFieldName, long* plValue )
{
	_variant_t vtValue;

	HRESULT hReturn = GetField( btFieldName, vtValue );

	if( hReturn == S_OK )
		*plValue = ( long )vtValue;

	return hReturn;
}

//	Get Field Helper Function for Text Type
HRESULT CMyADO::GetFieldText( _bstr_t btFieldName, char* szText, DWORD dwMaxTextSize )
{
	_variant_t vtValue;

	HRESULT hReturn = GetField( btFieldName, vtValue );

	if( hReturn == S_OK )
	{
		_bstr_t btValue = ( _bstr_t )vtValue;

		if( dwMaxTextSize < btValue.length())
			hReturn = S_FALSE;
		else
			strcpy( szText, btValue );
	}

	return hReturn;
}

//	Retrieve a Parameter (which was previously set up as either an Output or InputOutput Direction and is set during Stored Procedure Execution)
HRESULT CMyADO::GetParameter( _variant_t vtParameterName, _variant_t& vtValue )
{
	HRESULT hReturn = S_FALSE;

	ResetError();

	if( IsConnected() && IsInitialized() )
	{
		try
		{
			vtValue = m_pCommandPtr->Parameters->GetItem( vtParameterName )->Value;
		
			hReturn = S_OK;
		}
		catch( _com_error& eComError )
		{
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();
		}
	}

	return hReturn;
}

//	Retrieve Parameter Helper Function for Long Type
HRESULT CMyADO::GetParameterLong( _bstr_t btParameterName, long* plValue )
{
	_variant_t vtValue;

	HRESULT hReturn = GetParameter( btParameterName, vtValue );

	if( hReturn == S_OK )
		*plValue = ( long )vtValue;

	return hReturn;
}



//	Retrieve Parameter Helper Function for Return Value
HRESULT CMyADO::GetParameterReturnValue( long* plReturnValue )
{
	return GetParameterLong( "RETURN_VALUE", plReturnValue );
}

//	Retrieve Parameter Helper Function for Text Type
HRESULT CMyADO::GetParameterText( _bstr_t btParameterName, char* szText, DWORD dwMaxTextSize )
{
	_variant_t vtValue;

	HRESULT hReturn = GetParameter( btParameterName, vtValue );

	if( hReturn == S_OK )
	{
		_bstr_t btValue = ( _bstr_t )vtValue;

		if( dwMaxTextSize < btValue.length())
			hReturn = S_FALSE;
		else
			strcpy( szText, btValue );
	}

	return hReturn;
}

//	Retrieve the Record Count for the Recordset (which was created during Stored Procedure Execution)
HRESULT CMyADO::GetRecordCount( long* lRecordCount )
{
	HRESULT hReturn = S_FALSE;

	ResetError();

	if( m_pRecordsetPtr )
	{
		try
		{
			*lRecordCount = m_pRecordsetPtr->RecordCount;
		}
		catch( _com_error& eComError )
		{
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();
		}
	}

	return hReturn;
}


HRESULT CMyADO::GetFieldCount( long* lFieldCount )
{
	HRESULT hReturn = S_FALSE;

	ResetError();

	if( m_pRecordsetPtr )
	{
		try
		{
			*lFieldCount = m_pRecordsetPtr->GetFields()->Count;
		}
		catch( _com_error& eComError )
		{
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();
		}
	}

	return hReturn;
}

//	Close the Recordset and Initialize the CommandPtr Object
HRESULT CMyADO::Initialize( _bstr_t btStoredProcedureName )
{
	HRESULT hReturn = S_FALSE;

	if( IsConnected() )
	{
		m_nUseCount++;

		if( m_nUseCount >= 2 )
			m_nUseCount = m_nUseCount;		// 여러번 호출되는지 확인하기 위한 값

		m_pCommandPtr = NULL;

		if( m_pRecordsetPtr )
		{
			if( m_pRecordsetPtr->State == adStateOpen )
				m_pRecordsetPtr->Close();

			m_pRecordsetPtr = NULL;
		}

		m_pCommandPtr.CreateInstance( __uuidof( Command ));

		m_pCommandPtr->ActiveConnection = m_pConnectionPtr;
		m_pCommandPtr->CommandText = btStoredProcedureName;
		m_pCommandPtr->CommandType = adCmdStoredProc;

		hReturn = S_OK;
	}

	return hReturn;
}

//	Check for Connection Status
BOOL CMyADO::IsConnected()
{
	return ( m_pConnectionPtr );
}

//	Check for EOF on the Recordset (which was created during Stored Procedure Execution)
BOOL CMyADO::IsEOF()
{
	BOOL bReturn = TRUE;

	if( m_pRecordsetPtr )
		if( m_pRecordsetPtr->State == adStateOpen && !m_pRecordsetPtr->adoEOF )
			bReturn = FALSE;

	return bReturn;
}

//	Check for Initialization Status (CommandPtr Object is valid)
BOOL CMyADO::IsInitialized()
{
	return ( m_pCommandPtr );
}

//	Open a new ADO Connection
HRESULT CMyADO::Open( _bstr_t btConnectionString, _bstr_t btUserID, _bstr_t btPassword )
{
	HRESULT hReturn = S_FALSE;
	
	ResetError();

	if( m_pConnectionPtr == NULL )
	{
		m_pConnectionPtr.CreateInstance( __uuidof( Connection ));

		try
		{
			hReturn = m_pConnectionPtr->Open( btConnectionString, btUserID, btPassword, 0 );

			if( hReturn == S_OK )
				m_pConnectionPtr->CursorLocation = adUseClient;
		}
		catch( _com_error& eComError )
		{
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();
		}
	}
	else
		hReturn = S_OK;

	return hReturn;
}

//	Move to the Next Record in the Recordset (which was created during Stored Procedure Execution)
HRESULT CMyADO::MoveNext()
{
	HRESULT hResult = S_FALSE;
	
	ResetError();

	if( !IsEOF() )
	{
		try
		{
			hResult = m_pRecordsetPtr->MoveNext();
		}
		catch( _com_error& eComError )
		{
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();
		}
	}

	return hResult;
}

HRESULT CMyADO::Move( long nPos , long nStartType )
{
	HRESULT hResult = S_FALSE;
	
	ResetError();

	if( !IsEOF() )
	{
		try
		{
			_bstr_t btStartType = (long)( adBookmarkFirst );
			hResult = m_pRecordsetPtr->Move( nPos , _variant_t( btStartType ) );
		}
		catch( _com_error& eComError )
		{
			SetComError( eComError );
		}
		catch( ... )
		{
			SetGerError();
		}
	}

	return hResult;
}


void CMyADO::SetGerError()
{
	_snprintf( m_szLastErrorMsg , sizeof( m_szLastErrorMsg ), 
				"[Execute] error code(%d) ", GetLastError() );
}

void CMyADO::SetComError( _com_error& eComError )
{
	_snprintf( m_szLastErrorMsg , sizeof( m_szLastErrorMsg ), "%s %s \n", 
				eComError.ErrorMessage() , (char*)(LPCTSTR)eComError.Description() );
}

void CMyADO::ResetError()
{
	memset( &m_szLastErrorMsg ,0x00 , sizeof( m_szLastErrorMsg ) );
}

LPSTR CMyADO::GetErrorStr()
{
	return LPSTR( &m_szLastErrorMsg );
}
	
int CMyADO::IsErrorStr()
{
	return strlen( m_szLastErrorMsg );
}
