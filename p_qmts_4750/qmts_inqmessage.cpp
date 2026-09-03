/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      顾霞
Version:     1.0
Date:        2014-10-30
Description: 查询内部钢种
**************************************************/


//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/*<remark>=========================================================
/// <summary>
/// 查询各个制造标准
/// <para>
/// 获取输入参数：table_name(制造标准表名)/st_no(出钢记号)/whole_backlog_code(炼钢工序)
/// </para>
/// <para>数据库表：TQMTS0x(各个制造标准表)
///                 TQMTS01(工序制造标准表)
/// 				TQMTS02(工序成分标准表)
///                 TQMTS0X(工艺卡)
/// 前台各个QMTS0x画面的F2(查询)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各制造标准表    </param>
===========================================================</remark>*/

int f_qmtsp_00(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//校验工艺卡是否已更新

// service入口
BM2F_ENTERACE(qmts_inqmessage)


int f_qmts_inqmessage(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int fetchRowCount;
	CDecimal v_count = 0;

	CString table_name = " ";
	CString q_st_no = " ";

	CString q_whole_backlog_code = " ";
	CString catch_time = " ";
	int o_flag = 0;
	CString s_equ_no = " ";
	CString s_ingot_code = " ";
	CString base_code = "";

	/* 实体类定义 */
	CModel tep0002("TEP0002");
	CModel tqmts01("TQMTS01");
	CModel tqmts02("TQMTS02");
	CModel tqmts0x("TQMTS0X");
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_02(conn);

	EIClass bcls_rec_f;
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "st_no");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "factory_div");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "catch_time");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "base_code");

	//提示信息
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "if_pass");
	bcls_ret->Tables[0].Columns.Add(DT_STRING, "if_update");
	bcls_ret->Tables[0].Rows.Add();

	try
	{
		/*获得传入参数*/
		table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().Trim();
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();   ////update by yiling 20170224  加厂别
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();

		Log::Trace("", "", "qmts_inqmessage IN: st_no[{0}]whole_backlog_code[{1}]s_factory_div[{2}]base_code[{3}]", (const char*)q_st_no, (const char*)q_whole_backlog_code, tqmts0x["FACTORY_DIV"].ToString(), (const char*)base_code);
		if (bcls_rec->Tables[0].Columns.Contains("equ_no"))
		{
			s_equ_no = bcls_rec->Tables[0].Rows[0]["equ_no"].ToString().TrimOrBlank();
			Log::Trace("", "", "s_equ_no[{0}]", s_equ_no);
		}
		if (bcls_rec->Tables[0].Columns.Contains("ingot_code"))
		{
			s_ingot_code = bcls_rec->Tables[0].Rows[0]["ingot_code"].ToString().TrimOrBlank();
			Log::Trace("", "", "s_ingot_code[{0}]", s_ingot_code);
		}
		if (q_st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0006260")/*内部钢种不能为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tqmts0x["ST_NO"] = q_st_no;
		tqmts0x["BASE_CODE"] = base_code;
		tqmts0x.Query("ST_NO,FACTORY_DIV,BASE_CODE");
		//校验工艺卡内容是否更新
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT CATCH_TIME "
				"  FROM " + table_name +
				" WHERE ST_NO = @st_no "
			" AND FACTORY_DIV =@factory_div"
		    " AND BASE_CODE =@base_code"
				;
			break;
		}
		if (s_equ_no.TrimOrBlank() != " " || s_ingot_code.TrimOrBlank() != " ")
		{
			sqlstr = sqlstr + " AND EQU_NO=@equ_no AND INGOT_CODE=@ingot_code";
		}
		Log::Trace("", "", "sqlstr = [{0}]", (const char*)sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", q_st_no);
		cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("base_code", base_code);

		if (s_equ_no.TrimOrBlank() != " " || s_ingot_code.TrimOrBlank() != " ")
		{
			cmd_inq.Parameters.Set("equ_no", s_equ_no.TrimOrBlank());
			cmd_inq.Parameters.Set("ingot_code", s_ingot_code.TrimOrBlank());
		}
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			catch_time = cmd_inq.GetString(1);
		}
		Log::Trace("", "", "catch_time = [{0}]", (const char*)catch_time);
		cmd_inq.Close();

		bcls_rec_f.Tables[0].Rows.Add();
		bcls_rec_f.Tables[0].Rows[0]["st_no"] = q_st_no;
		bcls_rec_f.Tables[0].Rows[0]["factory_div"] = tqmts0x["FACTORY_DIV"];
		bcls_rec_f.Tables[0].Rows[0]["catch_time"] = catch_time;
		bcls_rec_f.Tables[0].Rows[0]["base_code"] = base_code;
		doFlag = f_qmtsp_00(&bcls_rec_f, &bcls_ret_f, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		o_flag = bcls_ret_f.Tables[0].Rows[0]["flag"];
		if (o_flag == 1)
		{
			bcls_ret->Tables[0].Rows[0]["if_update"] = "工艺卡内容已更新！请根据需要重新获取工艺卡。";
		}
		else
		{
			bcls_ret->Tables[0].Rows[0]["if_update"] = " ";
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT IF_PASS "
				"  FROM TQMTS01 "
				" WHERE ST_NO = @st_no "
				"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
				" AND FACTORY_DIV =@factory_div "
				" AND BASE_CODE =@base_code";

			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", q_st_no);
		cmd_inq.Parameters.Set("whole_backlog_code", q_whole_backlog_code);
		cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts01["IF_PASS"] = cmd_inq.GetString(1);
		}

		if (tqmts01["IF_PASS"].ToString() == "1")
		{
			bcls_ret->Tables[0].Rows[0]["if_pass"] = "制造标准已生效！";
		}
		else
		{
			bcls_ret->Tables[0].Rows[0]["if_pass"] = " ";
		}
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", "", "error=[{0}]", (const char*)str);

		strncpy(s.sysmsg, (const char*)str, 399);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
