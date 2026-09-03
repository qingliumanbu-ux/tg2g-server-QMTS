/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-13
Description: 试样成分实绩后备-试样修改
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///试样成分实绩后备-试样修改
/// <para>
/// 1.试样成分实绩后备-试样修改
/// 
/// </para>
/// <para>数据库表：TQMTS25(标准_工序成分)				</para>
/// <para>主调用函数：前台TQMTS25S的F4修改				</para>
/// <para>需调用函数：									</para>
/// </summary>
/// <param name="HEAT_NO">熔炼号						</param>
/// <param name="ST_SAMPLE_NO">试样号					</param>
/// <returns>  </returns>
===========================================================</remark>*/

/*  外部函数申明  */
int f_qmts_cf_rev(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);	//炼钢成分收集及判定

// service入口
BM2F_ENTERACE(qmts25s_upd)


int f_qmts25s_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	
  	CString sqlstr = "";
	
	/* 实体类定义 */
	CModel tqmts24("TQMTS24");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//定义传入参数块的列名
		bcls_rec->Tables[0].Columns.Add(DT_STRING,"ST_SAMPLE_SEQ");
		bcls_rec->Tables[0].Columns.Add(DT_STRING,"ELM_BACKUP");

		//--------------------------------------------------------
		/* 取得单行传入信息 */
		tqmts24.Reset();
		tqmts24.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		Log::Trace("", "","qmts25s_upd IN:---heat_no[{0}]",(const char*)tqmts24["HEAT_NO"].ToString());
		Log::Trace("", "","qmts25s_upd IN:---st_sample_no[{0}]",(const char*)tqmts24["ST_SAMPLE_NO"].ToString());
		Log::Trace("", "","qmts25s_upd IN:---analyse_time[{0}]",(const char*)tqmts24["ANALYSE_TIME"].ToString());
		Log::Trace("", "","qmts25s_upd IN:---c_s_judge_div[{0}]",(const char*)tqmts24["C_S_JUDGE_DIV"].ToString());
		Log::Trace("", "","qmts25s_upd IN:---sample_ifgood[{0}]",(const char*)tqmts24["SAMPLE_IFGOOD"].ToString());
		Log::Trace("", "", "qmts25s_upd IN:---ST_SAMPLE_DIV[{0}]", (const char*)tqmts24["ST_SAMPLE_DIV"].ToString());
		Log::Trace("", "", "qmts25s_upd IN:---REP_ELM_SEL_FLAG[{0}]", (const char*)tqmts24["REP_ELM_SEL_FLAG"].ToString());

		//校验传入参数
		if(tqmts24["HEAT_NO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg,_RES("QM00S0004334")/*熔炼号不允许为空。*/);  
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(tqmts24["ST_SAMPLE_NO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0004260")/*请选择或输入试样号。*/);  
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tqmts24["ST_SAMPLE_DIV"].ToString().Trim() == "1" && tqmts24["REP_ELM_SEL_FLAG"].ToString().Trim() == "1")
		{
			strcpy(s.msg, _RES("QM00S0006262")/*代表样不能修改。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT WHOLE_BACKLOG_CODE,ST_SAMPLE_DIV,GAS_TYPE_DIV,ST_SAMPLE_SEQ "
						 "  FROM TQMTS24 "
						 " WHERE HEAT_NO = @heat_no "
						 "   AND ST_SAMPLE_NO = @st_sample_no ";
				break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString().Trim());
		cmd_inq.Parameters.Set("st_sample_no", tqmts24["ST_SAMPLE_NO"].ToString().Trim());
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			tqmts24["WHOLE_BACKLOG_CODE"] = cmd_inq.GetString(1);
			tqmts24["ST_SAMPLE_DIV"] = cmd_inq.GetString(2);
			tqmts24["GAS_TYPE_DIV"] = cmd_inq.GetString(3);
			tqmts24["ST_SAMPLE_SEQ"] = cmd_inq.GetDecimal(4);
		}
		else
		{
			sprintf(s.msg, "熔炼号[{0}]试样号[{1}]的成分实绩不存在，请先新增！",(const char*)tqmts24["HEAT_NO"].ToString(),(const char*)tqmts24["ST_SAMPLE_NO"].ToString());
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		cmd_inq.Close();
		Log::Trace("", "","WHOLE_BACKLOG_CODE[{0}]",(const char*)tqmts24["WHOLE_BACKLOG_CODE"].ToString());
		Log::Trace("", "","ST_SAMPLE_DIV[{0}]",(const char*)tqmts24["ST_SAMPLE_DIV"].ToString());
		Log::Trace("", "","GAS_TYPE_DIV[{0}]",(const char*)tqmts24["GAS_TYPE_DIV"].ToString());
		Log::Trace("", "","ST_SAMPLE_SEQ[{0}]",tqmts24["ST_SAMPLE_SEQ"].ToDecimal().ToInt32());

		bcls_rec->Tables[0].Rows[0]["HEAT_NO"] = tqmts24["HEAT_NO"];
		bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
		bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"] = tqmts24["WHOLE_BACKLOG_CODE"];
		bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_DIV"] = tqmts24["ST_SAMPLE_DIV"];
		bcls_rec->Tables[0].Rows[0]["GAS_TYPE_DIV"] = tqmts24["GAS_TYPE_DIV"];
		bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_SEQ"] = tqmts24["ST_SAMPLE_SEQ"];
		bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME"] = tqmts24["ANALYSE_TIME"];
		bcls_rec->Tables[0].Rows[0]["SAMPLE_IFGOOD"] = "1";//画面后备的默认都是良样
		bcls_rec->Tables[0].Rows[0]["C_S_JUDGE_DIV"] = "1";//画面后备的默认都是经过C/S分析的
		bcls_rec->Tables[0].Rows[0]["ELM_BACKUP"] = "2";//画面后备

		doFlag = f_qmts_cf_rev(bcls_rec,bcls_ret,conn);
		if(doFlag != 0)
		{
			Log::Trace("", "","f_qmts_cf_rev() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
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
