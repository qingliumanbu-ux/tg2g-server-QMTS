/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      wsl
Version:     1.0
Date:        2023-12-05
Description: 修改炉次最终出钢记号 终判
**************************************************/


//框架公用头文件，勿删
#include "stdafx.h"



// service入口
BM2F_ENTERACE(qmts21_final_jud)
int f_qmts_stno_set(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_qmts21_final_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	int mat_count = 0;
	CDecimal mat_act_wt = 0;

	CString s_heat_no = " ";
	CString s_fin_st_no = " ";

	CString event_program = "ts21_force_jud";
	CString event_code = "100010";  //炉次改判
	CString keyvalue_1 = "熔炼号";
	CString keyvalue_2 = "终判出钢记号";
	CString keyvalue_3 = " ";
	CString proc_content = " ";

	CString sqlstr = "";
	CString sqlstr1 = "";


	/* 实体类定义 */
	CModel tqmts23("TQMTS23");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_upd(conn);


	try
	{
		//获得输入参数
		s_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		s_fin_st_no = bcls_rec->Tables[0].Rows[0]["fin_st_no"].ToString().Trim();

		Log::Trace("", "", "qmts21_final_jud IN:---s_heat_no = [{0}]", s_heat_no);
		Log::Trace("", "", "qmts21_final_jud IN:---s_fin_st_no = [{0}]", s_fin_st_no);

		//-----------合理性检查------------------
		if (s_heat_no.Trim() == "")
		{
			strcpy(s.msg, _RES("QM00S0004334")/*熔炼号不允许为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (s_fin_st_no.Trim() == "")
		{
			sprintf(s.msg, _RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//查询工艺卡是否存在该出钢记号，不存在则报错校验。
		CDecimal i_tqmts0x_num = 0;
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT COUNT(1) "
				"   FROM TQMTS0X "
				"  WHERE ST_NO = @st_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", s_fin_st_no.Trim());
		i_tqmts0x_num = cmd_inq.ExecuteScalar();
		cmd_inq.Close();
		if (i_tqmts0x_num <= 0)
		{
			CFormattable arguments[] = { s_fin_st_no }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("QM00S0004000")/*出钢记号[{0}]在工艺卡中不存在，不能操作。*/, arguments, 1); //格式化字符串
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//查询熔炼号是否已判定或者成分尚未选定，则报错校验。
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS23 "
				"  WHERE HEAT_NO = @heat_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", s_heat_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts23);
		}
		cmd_inq.Close();

		if (tqmts23["JUDGE_ST_NO"].ToString().Trim() != "")
		{
			CFormattable arguments[] = { (const char*)tqmts23["PONO"].ToString() }; // 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("QM00S0004290")/*制造命令号[{0}]已经做过终判，不能再终判。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}


		//强制终判，判定结果默认合
		tqmts23["JUDGE_CODE"] = "1";
		tqmts23["JUDGE_REMARK"] = "强制合格";
		tqmts23["FIN_ST_NO"] = tqmts23["ST_NO"];

		//初始化信息
		tqmts23["JUDGE_MAKER"] = s.userid;
		tqmts23["JUDGE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//将判定结果修改到TQMTS23
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " UPDATE TQMTS23 "
				" SET JUDGE_CODE = @judge_code, "
				" JUDGE_MAKER = @judge_maker, "
				" JUDGE_TIME = @judge_time, "
				" JUDGE_REMARK = @judge_remark, "
				" FIN_ST_NO = @fin_st_no, "
				" JUDGE_ST_NO = @judge_st_no"
				" WHERE HEAT_NO = @heat_no ";
			break;
		}
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("judge_code", tqmts23["JUDGE_CODE"].ToString());
		cmd_upd.Parameters.Set("judge_maker", tqmts23["JUDGE_MAKER"].ToString());
		cmd_upd.Parameters.Set("judge_time", tqmts23["JUDGE_TIME"].ToString());
		cmd_upd.Parameters.Set("judge_remark", tqmts23["JUDGE_REMARK"].ToString());
		cmd_upd.Parameters.Set("fin_st_no", s_fin_st_no.TrimOrBlank());
		cmd_upd.Parameters.Set("judge_st_no", s_fin_st_no.TrimOrBlank());
		cmd_upd.Parameters.Set("heat_no", s_heat_no);
		cmd_upd.ExecuteNonQuery();
		cmd_upd.Close();

		doFlag = f_qmts_stno_set(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", "", "炉次终判成功！");
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
