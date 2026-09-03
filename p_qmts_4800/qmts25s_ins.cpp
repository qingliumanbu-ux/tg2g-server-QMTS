/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-13
Description: 试样成分实绩后备-试样新增
**************************************************/

//框架公用头文件
#include "stdafx.h"

//程序用头文件


/*  外部函数申明  */
int f_qmts_cf_rev(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);	//炼钢成分收集及判定


/*<remark>=========================================================
/// <summary>
///试样成分实绩后备-试样新增
/// <para>
/// 1.试样成分实绩后备-试样新增
/// 
/// </para>
/// <para>数据库表：TQMTS25(标准_工序成分)				</para>
/// <para>主调用函数：前台TQMTS25S的F3新增				</para>
/// <para>需调用函数：									</para>
/// </summary>
/// <param name="HEAT_NO">熔炼号						</param>
/// <param name="ST_SAMPLE_NO">试样号					</param>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(qmts25s_ins)

int f_qmts25s_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0; 

	CString s_station_no = "";
	CString s_dev_code = "";
	CString s_charge_no = "";
	CString s_proc_no = "";
	CString s_plan_no = "";
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
		s_dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().TrimOrBlank();
		s_charge_no = bcls_rec->Tables[0].Rows[0]["CHARGE_NO"].ToString().TrimOrBlank();
		if (bcls_rec->Tables[0].Columns.Contains("PLAN_NO"))
		{
			s_plan_no = bcls_rec->Tables[0].Rows[0]["PLAN_NO"].ToString().TrimOrBlank();
		}
		Log::Trace("", "","qmts25s_ins IN:---heat_no[{0}]",(const char*)tqmts24["HEAT_NO"].ToString());
		Log::Trace("", "","qmts25s_ins IN:---st_sample_div[{0}]",(const char*)tqmts24["ST_SAMPLE_DIV"].ToString());
		Log::Trace("", "","qmts25s_ins IN:---gas_type_div[{0}]",(const char*)tqmts24["GAS_TYPE_DIV"].ToString());
		Log::Trace("", "","qmts25s_ins IN:---analyse_time[{0}]",(const char*)tqmts24["ANALYSE_TIME"].ToString());
		Log::Trace("", "","qmts25s_ins IN:---c_s_judge_div[{0}]",(const char*)tqmts24["C_S_JUDGE_DIV"].ToString());
		Log::Trace("", "","qmts25s_ins IN:---sample_ifgood[{0}]",(const char*)tqmts24["SAMPLE_IFGOOD"].ToString());
		Log::Trace("", "","qmts25s_ins IN:---s_dev_code[{0}]",(const char*)s_dev_code);
		Log::Trace("", "", "qmts25s_ins IN:---s_charge_no[{0}]", (const char*)s_charge_no);
		Log::Trace("", "", "qmts25s_ins IN:---s_plan_no[{0}]", (const char*)s_plan_no);

		//校验传入参数
		if(tqmts24["HEAT_NO"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg,_RES("QM00S0004334")/*熔炼号不允许为空。*/);  
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(s_dev_code.TrimOrBlank() == " ")
		{
			strcpy(s.msg,"设备不允许为空。");  
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(tqmts24["ST_SAMPLE_DIV"].ToString().TrimOrBlank() == " ")
		{
			strcpy(s.msg,"试样区分不允许为空。");  
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if(tqmts24["ST_SAMPLE_DIV"].ToString().TrimOrBlank() == "4")
		{
			if(tqmts24["GAS_TYPE_DIV"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,"气体区分不允许为空。");  
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//增加校验 update by quxiujuan 20190419
		//如果工序还没开始生产，则不可以新增成分
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "select PROC_NO "
				"  from TPSSM12 "
				" where FACTORY_DIV = @tqmts24.FACTORY_DIV "
				" and SM_PLAN_NO = @s_plan_no "
				" and CHARGE_NO = @s_charge_no "
				;
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts24.FACTORY_DIV", tqmts24["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("s_plan_no", s_plan_no);
		cmd_inq.Parameters.Set("s_charge_no", s_charge_no.Trim());

		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			s_proc_no = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		Log::Trace("", "", "s_proc_no = [{0}]", s_proc_no);

		//if (s_proc_no.TrimOrBlank() == " ")
		//{
		//	sprintf(s.msg, "熔炼号[%s]工序[%s]尚未开始生产，不可以新增此工序成分。", (const char*)tqmts24["HEAT_NO"].ToString(), (const char*)tqmts24["WHOLE_BACKLOG_CODE"].ToString());
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT STATION_ID,STATION_NO "
				"  FROM TPSSMD1 "
				" WHERE DEV_CODE = @dev_code "
				" ORDER BY AREA_ID desc ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("dev_code", s_dev_code.Trim());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			//tqmts24["WHOLE_BACKLOG_CODE"] = cmd_inq.GetString(1); ////同一设备能做多道工序，双联法。不能根据设备代码取工序。update by yiling 20161011
			s_station_no = cmd_inq.GetString(2);
		}
		cmd_inq.Close();
		Log::Trace("", "","STATION_NO = [{0}]",s_station_no);
		Log::Trace("", "","WHOLE_BACKLOG_CODE = [{0}]",tqmts24["WHOLE_BACKLOG_CODE"].ToString());

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT MAX(ST_SAMPLE_SEQ) "
				"  FROM TQMTS24 "
				" WHERE "
				"     HEAT_NO = @heat_no "
				"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
				"   AND ST_SAMPLE_DIV = @st_sample_div "
				"   AND GAS_TYPE_DIV = @gas_type_div ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString().Trim());
		cmd_inq.Parameters.Set("whole_backlog_code", tqmts24["WHOLE_BACKLOG_CODE"].ToString().Trim());
		cmd_inq.Parameters.Set("st_sample_div", tqmts24["ST_SAMPLE_DIV"].ToString().Trim());
		cmd_inq.Parameters.Set("gas_type_div", tqmts24["GAS_TYPE_DIV"].ToString());
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			Log::Trace("", "","已有样");
			tqmts24["ST_SAMPLE_SEQ"] = cmd_inq.GetDecimal(1)+1;
		}
		else
		{
			tqmts24["ST_SAMPLE_SEQ"] = 1;
		}
		cmd_inq.Close();
		Log::Trace("", "","st_sample_seq[{0}]",tqmts24["ST_SAMPLE_SEQ"].ToDecimal().ToInt32());

		//拼试样号——炼钢厂(1)+炼钢试样区分(1)+工序(1)+工位(1)+炼钢试样顺序号(1)+气体类型区分(1)  非气体样：气体类型区分写死"Z"  ///一个精炼设备，有可能追加LF工序，会有多条，加个charge_no号识别试样update by yiling 20161018
		if(tqmts24["ST_SAMPLE_DIV"].ToString().TrimOrBlank() != "4")
		{
			tqmts24["ST_SAMPLE_NO"] = "A" + tqmts24["ST_SAMPLE_DIV"].ToString().Trim() + tqmts24["WHOLE_BACKLOG_CODE"].ToString() + s_station_no.Trim() + tqmts24["ST_SAMPLE_SEQ"].ToDecimal().ToString().Trim() + s_charge_no + "Z";
		}
		else
		{
			tqmts24["ST_SAMPLE_NO"] = "A" + tqmts24["ST_SAMPLE_DIV"].ToString().Trim() + tqmts24["WHOLE_BACKLOG_CODE"].ToString() + s_station_no.Trim() + tqmts24["ST_SAMPLE_SEQ"].ToDecimal().ToString().Trim() + s_charge_no + tqmts24["GAS_TYPE_DIV"].ToString().Trim();
		}
		Log::Trace("", "","st_sample_no[{0}]",(const char*)tqmts24["ST_SAMPLE_NO"].ToString());

		bcls_rec->Tables[0].Rows[0]["HEAT_NO"] = tqmts24["HEAT_NO"];
		bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
		bcls_rec->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"] = tqmts24["WHOLE_BACKLOG_CODE"];
		bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_DIV"] = tqmts24["ST_SAMPLE_DIV"];
		bcls_rec->Tables[0].Rows[0]["GAS_TYPE_DIV"] = tqmts24["GAS_TYPE_DIV"];
		bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME"] = tqmts24["ANALYSE_TIME"];
		bcls_rec->Tables[0].Rows[0]["C_S_JUDGE_DIV"] = "1";//画面后备的默认都是经过C/S分析的
		bcls_rec->Tables[0].Rows[0]["SAMPLE_IFGOOD"] = "1";//画面后备的默认都是良样
		bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_SEQ"] = tqmts24["ST_SAMPLE_SEQ"];
		bcls_rec->Tables[0].Rows[0]["ELM_BACKUP"] = "2";//画面后备
		bcls_rec->Tables[0].Rows[0]["REP_ELM_SEL_FLAG"] = " ";  ///要不然新增时,会把已经选代表成分的标记带过来
		doFlag = f_qmts_cf_rev(bcls_rec,bcls_ret,conn);
		if(doFlag != 0)
		{
			Log::Trace("", "","f_qmts_cf_rev() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//定义传出参数块的列名
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"WHOLE_BACKLOG_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ST_SAMPLE_NO");
		bcls_ret->Tables[0].Rows.Add();
		bcls_ret->Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"] = tqmts24["WHOLE_BACKLOG_CODE"];
		bcls_ret->Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
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

