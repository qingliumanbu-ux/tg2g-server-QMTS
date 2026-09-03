/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      林阿贰
Version:     1.0
Date:        2015-12-10
Description: 将HEAT_NO_OLD的成分信息复制到HEAT_NO上，并根据ST_NO的标准判定
目前没有哪里调用本函数！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
**************************************************/

//框架公用头文件
#include "stdafx.h"

//程序用头文件


/*<remark>=========================================================
/// <summary>
///将HEAT_NO_OLD的成分信息复制到HEAT_NO上
/// <para>
/// 1.将HEAT_NO_OLD的成分信息复制到HEAT_NO上
/// </summary>
/// <param name="HEAT_NO_OLD">源熔炼号					</param>
/// <param name="HEAT_NO">熔炼号						</param>
/// <param name="PONO">熔炼号							</param>
/// <param name="ST_NO">出钢记号						</param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口
BM2_FUNCTION_EXPORT
int f_qmts_cf_copy(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString s_heat_no_old = "";
	CString s_heat_no = "";
	CString s_st_no = "";
	CString s_pono = "";

	CString s_station_no = "";
	CString s_dev_code = "";

	CString sqlstr = "";
	CString sqlstr_tqq0 = "";

	int elm_num = 0;//实绩元素数量

	/* 实体类定义 */
	CModel tqmtqq0("TQMTQQ0");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_tqq0(conn);


	try
	{
		//获取传入参数
		s_heat_no_old = bcls_rec->Tables[0].Rows[0]["HEAT_NO_OLD"].ToString().TrimOrBlank();
		s_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank();
		s_pono = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().TrimOrBlank();
		s_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().TrimOrBlank();
		Log::Trace("", "", "get-->s_heat_no_old[{0}]", (const char*)s_heat_no_old);
		Log::Trace("", "", "get-->s_heat_no[{0}]", (const char*)s_heat_no);
		Log::Trace("", "", "get-->s_pono[{0}]", (const char*)s_pono);
		Log::Trace("", "", "get-->s_st_no[{0}]", (const char*)s_st_no);

		//校验传入参数
		if (s_heat_no_old.TrimOrBlank() == " ")
		{
			strcpy(s.msg, "复制源熔炼号不允许为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (s_heat_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg, "熔炼不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (s_pono.TrimOrBlank() == " ")
		{
			strcpy(s.msg, "制造命令号不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (s_st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg, "出钢记号不允许为空。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//查询工序下的试样信息
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr_tqq0 = "SELECT * "
				"  FROM TQMTQQ0 "
				" WHERE HEAT_NO = @s_heat_no_old ";
			break;
		}
		cmd_inq_tqq0.SetCommandText(sqlstr_tqq0);
		cmd_inq_tqq0.Parameters.Set("s_heat_no_old", s_heat_no_old);
		cmd_inq_tqq0.ExecuteReader();
		while (cmd_inq_tqq0.Read())
		{
			tqmtqq0.Reset();
			cmd_inq_tqq0.Fetch(tqmtqq0);

			tqmtqq0["HEAT_NO"] = s_heat_no;
			tqmtqq0["PONO"] = s_pono;
			tqmtqq0["ST_NO"] = s_st_no;

			tqmtqq0["REC_CREATOR"] = s.userid;
			tqmtqq0["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmtqq0["REC_REVISOR"] = " ";
			tqmtqq0["REC_REVISE_TIME"] = " ";
			tqmtqq0["ARCHIVE_FLAG"] = " ";

			//插入TQMTS24试样主记录
			tqmtqq0.TrimOrBlank();
			Log::Trace("", __FUNCTION__, "tqmtqq0.Insert()");
			elm_num++;//记录新增元素数据
			tqmtqq0.Delete("HEAT_NO,ELM_CODE");
			tqmtqq0.Insert();
			//strcpy(s.msg, "新增熔炼号[" + tqmtqq0["HEAT_NO"].ToString() + "]制造命令号[" + tqmtqq0["PONO"].ToString() + "]出钢记号[" + tqmtqq0["ST_NO"].ToString() + "]元素代码[" + tqmtqq0["ELM_CODE"].ToString() + "]元素名称[" + tqmtqq0["ELM_NAME"].ToString() + "]的成分信息");
				
		}
		cmd_inq_tqq0.Close();
		if (elm_num<=0)
		{
			strcpy(s.msg, "获取不到熔炼号[" + s_heat_no_old + "]的成分信息");
			throw CApplicationException(-1, s.msg, log.Location);
		}
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

