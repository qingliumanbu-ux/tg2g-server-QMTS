/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      
Version:     1.0
Date:        2023-05-25
Description: 炉次成分主信息查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炉次成分主信息查询
/// <para>
/// 1.炉次成分主信息查询
/// 
/// </para>
/// <para>数据库表：TQMTS24(实绩_炉次成分主信息)			</para>
/// <para>主调用函数：前台QMTS24M画面的gridView_main点击调用		</para>
/// <para>需调用函数：							</para>
/// </summary>
/// <param name="HEAT_NO">  熔炼号				</param>
/// <returns>  </returns>
===========================================================</remark>*/


// service入口
BM2F_ENTERACE(qmts21m_inq_24)


int f_qmts21m_inq_24(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CString q_heat_no = "";
	CString s_judge_code = "";
	

	CString sqlstr = "";
	
	/* 实体类定义 */
	CModel tqmts24("TQMTS24");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		q_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		//s_judge_code = bcls_rec->Tables[0].Rows[0]["judge_code"].ToString().Trim();
		Log::Trace("", "","qmts21m_inq_24 IN:---q_heat_no = [{0}]",(const char*)q_heat_no);
		//Log::Trace("", "", "qmts21m_inq_24 IN:---s_judge_code = [{0}]", s_judge_code);

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
						 "  FROM TQMTS24 "
						 "  WHERE HEAT_NO like '%'||@heat_no||'%' ";
				//if (s_judge_code.Trim() != "")  sqlstr = sqlstr + " AND JUDGE_CODE = @judge_code ";
				break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", q_heat_no);
		//cmd_inq.Parameters.Set("judge_code", s_judge_code);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		
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
