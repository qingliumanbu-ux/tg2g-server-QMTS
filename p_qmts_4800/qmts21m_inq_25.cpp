/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:
Version:     1.0
Date:        2023-05-25
Description: 炉次成分子信息查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炉次成分子信息查询
/// <para>
/// 1.炉次成分子信息查询
///
/// </para>
/// <para>数据库表：TQMTS25(实绩_炉次成分子信息)			</para>
/// <para>主调用函数：前台QMTS25M画面的gridView1点击调用		</para>
/// <para>需调用函数：							</para>
/// </summary>
/// <param name="HEAT_NO">  熔炼号				</param>
/// <param name="ST_SAMPLE_NO">  	试样号  	</param>
/// <returns>  </returns>
===========================================================</remark>*/


// service入口
BM2F_ENTERACE(qmts21m_inq_25)


int f_qmts21m_inq_25(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	CString q_heat_no = " ";
	CString q_st_sample_no = " ";
	//CString q_whole_backlog_code = " ";

	CString sqlstr = "";
	CString sqlstrStd = "";
	CString st_no = "";
	CString elm_code = "";
	/* 实体类定义 */
	CModel tqmts25("TQMTS25");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	try
	{
		q_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		q_st_sample_no = bcls_rec->Tables[0].Rows[0]["st_sample_no"].ToString().Trim();
		//q_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		Log::Trace("", "", "qmts21m_inq_24 IN:---q_heat_no = [{0}]", (const char*)q_heat_no);
		Log::Trace("", "", "qmts21m_inq_24 IN:---q_st_sample_no = [{0}]", (const char*)q_st_sample_no);
		//Log::Trace("", "", "qmts21m_inq_24 IN:---q_whole_backlog_code = [{0}]", (const char*)q_whole_backlog_code);
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TQMTS25 "
				" WHERE HEAT_NO = @HEAT_NO AND ST_SAMPLE_NO = @ST_SAMPLE_NO  ";
			break;
		}

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("HEAT_NO", q_heat_no);
		cmd_inq.Parameters.Set("ST_SAMPLE_NO", q_st_sample_no);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();
		Log::Trace("", "", "开始查询标准信息");
	/*	bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAIN_MIN");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAIN_MAX");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "MAIN_AIM");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SPE_MIN");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "SPE_MAX");*/
		//for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		//{
		//	st_no = bcls_ret->Tables[0].Rows[i]["ST_NO"];
		//	elm_code = bcls_ret->Tables[0].Rows[i]["ELM_CODE"];
		//	Log::Trace("", "", "qmts21m_inq_24 IN:---ST_NO = [{0}]", (const char*)st_no);
		//	Log::Trace("", "", "qmts21m_inq_24 IN:---ELM_CODE = [{0}]", (const char*)elm_code);
		//	switch (conn->DatabaseKind)
		//	{
		//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:				// MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//	default:						// 所有数据库适用，通用SQL语句
		//		sqlstrStd = "SELECT MAIN_MIN,MAIN_MAX,MAIN_AIM,SPE_MIN,SPE_MAX "
		//			"  FROM TQMTS02 "
		//			" WHERE ST_NO = @ST_NO AND ELM_CODE = @ELM_CODE AND WHOLE_BACKLOG_CODE =@q_whole_backlog_code ";
		//		break;
		//	}
		//	cmd_inq.SetCommandText(sqlstrStd);
		//	cmd_inq.Parameters.Set("ST_NO", st_no);
		//	cmd_inq.Parameters.Set("ELM_CODE", elm_code);
		//	cmd_inq.Parameters.Set("q_whole_backlog_code", q_whole_backlog_code);
		//	cmd_inq.ExecuteReader();
		//	if (cmd_inq.Read())
		//	{
		//		bcls_ret->Tables[0].Rows[i]["MAIN_MIN"] = cmd_inq.GetDecimal(1);
		//		bcls_ret->Tables[0].Rows[i]["MAIN_MAX"] = cmd_inq.GetDecimal(2);
		//		bcls_ret->Tables[0].Rows[i]["MAIN_AIM"] = cmd_inq.GetDecimal(3);
		//		bcls_ret->Tables[0].Rows[i]["SPE_MIN"] = cmd_inq.GetDecimal(4);
		//		bcls_ret->Tables[0].Rows[i]["SPE_MAX"] = cmd_inq.GetDecimal(5);
		//	}
		//	cmd_inq.Close();
		//}
		

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
