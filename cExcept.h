//		cExcept.h
#ifndef M_EXCEPTION			// ���� ó�� ��ũ�� 
#define M_EXCEPTION

	#ifndef ME_APP_NAME
		#define ME_APP_NAME	"ShowException"
	#endif

#include <stdio.h>


#define EPER( EP , member ) ( EP->ExceptionRecord->member )		//	 �� �ڵ带 �� ������...

#include <crtdbg.h>

#ifdef _DEBUG
//	#define ME_SHOW_TRACE
#endif


#ifndef _DEF_SHOWEXCEPTION
#define _DEF_SHOWEXCEPTION
// ����ó�� ���� �����ֱ�
void ShowException( unsigned int nErr , EXCEPTION_POINTERS* lpError , LPSTR lpszFunctionName )
{
#ifdef ME_SHOW_TRACE	// ����� ����� ��� TRACE �� �����ֱ⸸ �Ѵ�.
	
	//  Address ,  Code , Flags
	_RPTF2( _CRT_WARN , "%s[%s] \n" , ME_APP_NAME , lpszFunctionName );

    _RPTF3( _CRT_WARN , " Address(%x) Code(%x) Flags(%x) \n" ,		
		EPER( lpError , ExceptionAddress ) ,
		EPER( lpError , ExceptionCode ) ,
		EPER( lpError , ExceptionFlags ) );

#else

	char szText[ 200 ] = {0};		//	12 +1 +12 +12 +20 +100
	//sprintf( szText , #FunctionName "(%x)"  , dwError );

	sprintf( szText , "[%s] Address(%x) Code(%x) Flags(%x) " , lpszFunctionName , 
		EPER( lpError , ExceptionAddress ) ,
		EPER( lpError , ExceptionCode ) ,
		EPER( lpError , ExceptionFlags ) );
	
	MessageBox( NULL , szText , ME_APP_NAME , MB_OK );

#endif

}


#endif


DWORD except_case( unsigned int nErr , LPEXCEPTION_POINTERS lpError , LPSTR lpszFunctionName )
{
	ShowException( nErr , lpError , lpszFunctionName );
	
	//EXCEPTION_CONTINUE_EXECUTION (-1) 
	//���ܸ� �����ϰ�, ���ܰ� �߻��� ������������ ���α׷��� ��� �����Ѵ�. 
	//���� ��� 10 / i ���� i�� 0�̶� ���ܰ� �߻��� ���, ���� ó�� ���Ͱ� �� ���̶��, �ٽ� 10 / i���� �����Ѵٴ� ���̴�. 

	//EXCEPTION_CONTINUE_SEARCH (0) 
	//except ��� ���� �ڵ带 �������� �ʰ�, ������ �ִ� ���� ó�� �ڵ鷯���� ���� ó���� �ѱ��. 

	//EXCEPTION_EXECUTE_HANDLER (1) 
	//except ��� ���� �ڵ带 ������ ��, except ��� �Ʒ��� �ڵ带 �����Ѵ�.

	return EXCEPTION_EXECUTE_HANDLER;
}


/////////////////////////////////////////////
//	for C
/////////////////////////////////////////////

#define _STC	__try		{

/////////////////////////////////////////////	EnD	C		fn : Function , exc : Execute code
#define _EDC( fn , exc )	}		\
__except( except_case( GetExceptionCode() , GetExceptionInformation() , #fn ) )	\
{						\
	exc					\
}
/////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////
//		for	C++
//////////////////////////////////////////////////////////////////////////////

	#ifdef __cplusplus
	#include <eh.h>

		class SE_Exception
		{
		private:
			unsigned int nSE;
			EXCEPTION_POINTERS* m_lpError;
		public:
			SE_Exception() {}
			SE_Exception( unsigned int n ) : nSE( n ) {}
			SE_Exception( unsigned int n , EXCEPTION_POINTERS* lpError ) : nSE( n ) , m_lpError( lpError )  {}

			~SE_Exception() {}
			unsigned int getSeNumber() { return nSE; }
			EXCEPTION_POINTERS*	getInfomation()	{	return m_lpError;	}

			static void trans_func( unsigned int nErr , EXCEPTION_POINTERS* lpError )
			{
		//		ShowException( nErr , lpError );
		//		throw SE_Exception( nErr );
				throw SE_Exception( nErr , lpError );
			}

			static void ShowCatchAny( LPSTR lpszFunctionName )
			{
				#ifdef ME_SHOW_TRACE	// ����� ����� ��� TRACE �� �����ֱ⸸ �Ѵ�.
				
					_RPTF2( _CRT_WARN , "%s[%s]  catch(...)  \n" , ME_APP_NAME , lpszFunctionName );					

				#else

					char szText[ 200 ] = {0};		//	12 +1 +12 +12 +20 +100
					sprintf( szText , "[%s] catch(...) " , lpszFunctionName );					
					MessageBox( NULL , szText , ME_APP_NAME , MB_OK );

				#endif
			}

			static void ShowCatchNum( int nCode , LPSTR lpszFunctionName )
			{
				#ifdef ME_SHOW_TRACE	// ����� ����� ��� TRACE �� �����ֱ⸸ �Ѵ�.
				
					_RPTF3( _CRT_WARN , "%s[%s]  catch(int) =>%d  \n" , ME_APP_NAME , lpszFunctionName , nCode );					

				#else

					char szText[ 200 ] = {0};		//	12 +1 +12 +12 +20 +100
					sprintf( szText , "[%s] catch(int) =>%d " , lpszFunctionName , nCode );					
					MessageBox( NULL , szText , ME_APP_NAME , MB_OK );

				#endif
			}

			static void ShowCatchStr( char* pstr , LPSTR lpszFunctionName )
			{
				#ifdef ME_SHOW_TRACE	// ����� ����� ��� TRACE �� �����ֱ⸸ �Ѵ�.
				
					_RPTF3( _CRT_WARN , "%s[%s]  catch(char*) =>%s  \n" , ME_APP_NAME , lpszFunctionName , pstr );						

				#else

					char szText[ 300 ] = {0};		//	12 +1 +12 +12 +20 +100
					if( strlen( pstr ) > 200 )
						sprintf( szText , "[%s] catch(char*) =>len(%d) over " , lpszFunctionName , strlen( pstr ) );					
					else
						sprintf( szText , "[%s] catch(char*) =>%s " , lpszFunctionName , pstr );					
					MessageBox( NULL , szText , ME_APP_NAME , MB_OK );

				#endif
			}
		};


		/////////////////////////////////////////////
		#define	_INIT_STCPP		_set_se_translator( SE_Exception::trans_func );	
		#define	_STCPP			try    {
		
		/////////////////////////////////////////////


		#define	_EDCPP( fn , exc )		}									\
		catch( SE_Exception e )												\
		{																	\
			ShowException( e.getSeNumber() , e.getInfomation() , #fn );		\
			exc																\
		}		
		
		//	���ڿ� throw ��� 
		#define	_EDCPP_STR( fn , exc )										\
		catch( char* pstr )													\
		{																	\
			SE_Exception::ShowCatchStr( pstr , #fn );						\
			exc																\
		}
		
		// ���� throw ���
		#define	_EDCPP_NUM( fn , exc )										\
		catch( int nCode )													\
		{																	\
			SE_Exception::ShowCatchNum( nCode , #fn );						\
			exc																\
		}

		// ������ �� ��� 
		#define	_EDCPP_ANY( fn , exc )										\
		catch( ... )														\
		{																	\
			SE_Exception::ShowCatchAny( #fn );								\
			exc																\
		}		
		

		#define	_EDCPP_ALL( fn , exc )										\
		_EDCPP( fn , exc )													\
		_EDCPP_STR( fn , exc )												\
		_EDCPP_NUM( fn , exc )												\
		_EDCPP_ANY( fn , exc )												\

		//////////////////////////////////////////////

	#endif				

#endif



/*
// ��� ����

void main()
{
_STC
...
����


_EDC( CQuery::OnPOOL_LOGIN , return 0; )
}


void winmain()
{
_INIT_STCPP
_STCPP
...
����


_EDCPP_ALL( CQuery::OnPOOL_LOGIN , return 0; )

}


*/
