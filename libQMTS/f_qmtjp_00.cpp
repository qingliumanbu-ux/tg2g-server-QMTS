/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-05
Description: 操作履历_入口函数
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// 操作履历_入口函数
/// <para>
/// 1.操作履历_入口函数。
/// 
/// </para>
/// <para>数据库表： TQMTS99(跟踪信息记录表)						            </para>
/// <para>主调用函数：前台QMTS21\QMTS22的判定以及QMTS27画面的增删改调用	        </para>
/// <para>需调用函数：															<para>
/// </summary>
/// <param name>  </param>
/// <returns>  </returns>
===========================================================</remark>*/


BM2_FUNCTION_EXPORT
 int f_qmtjp_00(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts99("TQMTS99");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		////获得输入参数
		////*********************************   1. 获取/检查输入参数   *********************************//
		tqmts99["EVENT_PROGRAM"] = bcls_rec->Tables[0].Rows[0]["EVENT_PROGRAM"].ToString().Trim(); //事件相关程序
		tqmts99["EVENT_CODE"] = bcls_rec->Tables[0].Rows[0]["EVENT_CODE"].ToString().Trim(); //事件代码
		tqmts99["KEYVALUE_1"] = bcls_rec->Tables[0].Rows[0]["KEYVALUE_1"].ToString().Trim(); //关键字串1
		tqmts99["KEYVALUE_2"] = bcls_rec->Tables[0].Rows[0]["KEYVALUE_2"].ToString().Trim(); //关键字串2
		tqmts99["KEYVALUE_3"] = bcls_rec->Tables[0].Rows[0]["KEYVALUE_3"].ToString().Trim(); //关键字串3
		tqmts99["PROC_CONTENT"] = bcls_rec->Tables[0].Rows[0]["PROC_CONTENT"].ToString().Trim(); //处理内容
		tqmts99["EVENT_DATETIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss"); //发生时刻
		tqmts99["EVENT_MAKER"] = s.userid; //事件责任者

		if (bcls_ret->Tables[0].Columns.Contains("MAT_ACT_WT"))
		{
			tqmts99["MAT_ACT_WT"] = bcls_ret->Tables[0].Columns.Contains("MAT_ACT_WT");
		}

		Log::Trace("", __FUNCTION__, "f_qmtjp_00 IN:---event_datetime  	= [{0}]",(const char*)tqmts99["EVENT_DATETIME"].ToString() 	); 
		Log::Trace("", __FUNCTION__, "f_qmtjp_00 IN:---event_maker   		= [{0}]",(const char*)tqmts99["EVENT_MAKER"].ToString()     	);
		Log::Trace("", __FUNCTION__, "f_qmtjp_00 IN:---event_program    	= [{0}]",(const char*)tqmts99["EVENT_PROGRAM"].ToString()	    );
		Log::Trace("", __FUNCTION__, "f_qmtjp_00 IN:---event_code         	= [{0}]",(const char*)tqmts99["EVENT_CODE"].ToString()        	);
		Log::Trace("", __FUNCTION__, "f_qmtjp_00 IN:---keyvalue_1       	= [{0}]",(const char*)tqmts99["KEYVALUE_1"].ToString()      	);
		Log::Trace("", __FUNCTION__, "f_qmtjp_00 IN:---keyvalue_2       	= [{0}]",(const char*)tqmts99["KEYVALUE_2"].ToString()      	);
		Log::Trace("", __FUNCTION__, "f_qmtjp_00 IN:---keyvalue_3       	= [{0}]",(const char*)tqmts99["KEYVALUE_3"].ToString()      	);
		Log::Trace("", __FUNCTION__, "f_qmtjp_00 IN:---proc_content 		= [{0}]",(const char*)tqmts99["PROC_CONTENT"].ToString()		);
		Log::Trace("", __FUNCTION__, "f_qmtjp_00 IN:-- -tqmts99.MAT_ACT_WT  = [{0}]", tqmts99["MAT_ACT_WT"].ToDecimal());

		// 获取顺序号
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT NVL(MAX(SEQ_ID)+1,0) FROM TQMTS99 ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts99["SEQ_ID"] = cmd_inq.GetDecimal(1);
		}
		cmd_inq.Close();
		//CString seq_id = EPGetNextSeq("QMTS99_SEQ_ID", conn);
		//tqmts99["SEQ_ID"] = atoi((const char*)seq_id);
		Log::Trace("", __FUNCTION__, "seq_id = [{0}]",tqmts99["SEQ_ID"].ToDecimal().ToInt32()); 

		//switch(conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:						// 所有数据库适用，通用SQL语句
		//	sqlstr = " SELECT NVL(MAX(event_sub_system),' '),NVL(MAX(grade_div),'0'), NVL(MAX(event_desc),' ') "
		//		" FROM TQMTS98 "
		//		" WHERE EVENT_CODE = @event_code ";
		//	break;
		//}
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("event_code",tqmts99["EVENT_CODE"].ToString());
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	tqmts99["EVENT_SUB_SYSTEM"] = cmd_inq.GetString(1);
		//	tqmts99["GRADE_DIV"] = cmd_inq.GetString(1);
		//	tqmts99["EVENT_SUB_SYSTEM"] = cmd_inq.GetString(1);
		//}
		//cmd_inq.Close();
		Log::Trace("", __FUNCTION__, "event_sub_system = [{0}]",(const char*)tqmts99["EVENT_SUB_SYSTEM"].ToString()   );
		Log::Trace("", __FUNCTION__, "grade_div       	= [{0}]",(const char*)tqmts99["GRADE_DIV"].ToString()      	);
		Log::Trace("", __FUNCTION__, "event_desc		= [{0}]",(const char*)tqmts99["EVENT_DESC"].ToString()			);

		//写入跟踪信息履历表
		tqmts99.TrimOrBlank();
		tqmts99.Insert();
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

