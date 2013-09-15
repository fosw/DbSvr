//		cExcept.h
#ifndef M_EXCEPTION			// 예외 처리 매크로 
#define M_EXCEPTION

	#ifndef ME_APP_NAME
		#define ME_APP_NAME	"ShowException"
	#endif

#include <stdio.h>


#define EPER( EP , member ) ( EP->ExceptionRecord->member )		//	 긴 코드를 좀 줄이자...

#include <crtdbg.h>

#ifdef _DEBUG
//	#define ME_SHOW_TRACE
#endif


#ifndef _DEF_SHOWEXCEPTION
#define _DEF_SHOWEXCEPTION
// 예외처리 내용 보여주기
void ShowException( unsigned int nErr , EXCEPTION_POINTERS* lpError , LPSTR lpszFunctionName )
{
#ifdef ME_SHOW_TRACE	// 디버그 모드일 경우 TRACE 에 보여주기만 한다.
	
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
	//예외를 무시하고, 예외가 발생한 지점에서부터 프로그램을 계속 실행한다. 
	//예를 들어 10 / i 에서 i가 0이라서 예외가 발생한 경우, 예외 처리 필터가 이 값이라면, 다시 10 / i부터 실행한다는 말이다. 

	//EXCEPTION_CONTINUE_SEARCH (0) 
	//except 블록 안의 코드를 실행하지 않고, 상위에 있는 예외 처리 핸들러에게 예외 처리를 넘긴다. 

	//EXCEPTION_EXECUTE_HANDLER (1) 
	//except 블록 안의 코드를 실행한 후, except 블록 아래의 코드를 실행한다.

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
				#ifdef ME_SHOW_TRACE	// 디버그 모드일 경우 TRACE 에 보여주기만 한다.
				
					_RPTF2( _CRT_WARN , "%s[%s]  catch(...)  \n" , ME_APP_NAME , lpszFunctionName );					

				#else

					char szText[ 200 ] = {0};		//	12 +1 +12 +12 +20 +100
					sprintf( szText , "[%s] catch(...) " , lpszFunctionName );					
					MessageBox( NULL , szText , ME_APP_NAME , MB_OK );

				#endif
			}

			static void ShowCatchNum( int nCode , LPSTR lpszFunctionName )
			{
				#ifdef ME_SHOW_TRACE	// 디버그 모드일 경우 TRACE 에 보여주기만 한다.
				
					_RPTF3( _CRT_WARN , "%s[%s]  catch(int) =>%d  \n" , ME_APP_NAME , lpszFunctionName , nCode );					

				#else

					char szText[ 200 ] = {0};		//	12 +1 +12 +12 +20 +100
					sprintf( szText , "[%s] catch(int) =>%d " , lpszFunctionName , nCode );					
					MessageBox( NULL , szText , ME_APP_NAME , MB_OK );

				#endif
			}

			static void ShowCatchStr( char* pstr , LPSTR lpszFunctionName )
			{
				#ifdef ME_SHOW_TRACE	// 디버그 모드일 경우 TRACE 에 보여주기만 한다.
				
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
		
		//	문자열 throw 잡기 
		#define	_EDCPP_STR( fn , exc )										\
		catch( char* pstr )													\
		{																	\
			SE_Exception::ShowCatchStr( pstr , #fn );						\
			exc																\
		}
		
		// 숫자 throw 잡기
		#define	_EDCPP_NUM( fn , exc )										\
		catch( int nCode )													\
		{																	\
			SE_Exception::ShowCatchNum( nCode , #fn );						\
			exc																\
		}

		// 무조건 다 잡기 
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
// 사용 예시

void main()
{
_STC
...
내용


_EDC( CQuery::OnPOOL_LOGIN , return 0; )
}


void winmain()
{
_INIT_STCPP
_STCPP
...
내용


_EDCPP_ALL( CQuery::OnPOOL_LOGIN , return 0; )

}


*/
