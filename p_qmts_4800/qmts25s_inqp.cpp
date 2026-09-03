/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-17
Description: 试样成分实绩后备-炉次查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
///试样成分实绩后备-炉次查询
/// <para>
/// 1.试样成分实绩后备-炉次查询
/// </para>
/// <para>数据库表：tpssm11(炼钢作业计划炉次钢种管理表)		</para>
/// <para>主调用函数：前台TQMTS25S的F2查询				    </para>
/// <para>需调用函数：										</para>
/// </summary>
/// <returns>  </returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts25s_inqp)


int f_qmts25s_inqp(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString cast_div_no_2 = "";
	CString cast_no_show;
	CDecimal pono_status;                    /* 制造命令状态 */
	CString sg_sign = "";
	CString s_rep_elm_sel_flag = "";
	CString sqlstr = "";
	CString s_factory_div = "";  //2023.1.16

	/* 实体类定义 */
	CModel tpssm11("TPSSM11");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);
	CDbCommand cmd_inq_02(conn);

	try
	{
		//定义返回块的列名
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SG_SIGN");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "PONO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REFINE_ROUTE_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_DECIMAL, "PONO_STATUS");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "CAST_NO_SHOW");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REP_ELM_SEL_FLAG");

		/* 获得输入参数 */
		tpssm11["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		tpssm11["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();

		if(bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV")) s_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();

		Log::Trace("", "", "qmts25s_inqp IN:---HEAT_NO = [{0}]", tpssm11["HEAT_NO"].ToString());
		Log::Trace("", "", "qmts25s_inqp IN:---PONO = [{0}]", tpssm11["PONO"].ToString());
		Log::Trace("", "", "qmts25s_inqp IN:---FACTORY_DIV = [{0}]", s_factory_div);

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * FROM "
				"(SELECT * FROM TPSSM11 WHERE 1=1  ";
			if (s_factory_div != "") sqlstr += " AND FACTORY_DIV = '" + s_factory_div + "' ";   //2023.1.16
			if (tpssm11["HEAT_NO"].ToString().Trim() != "")	sqlstr += " AND HEAT_NO like '%" + tpssm11["HEAT_NO"].ToString().Trim() + "%' ";
			if (tpssm11["PONO"].ToString().Trim() != "")		sqlstr += " AND PONO    like '%" + tpssm11["PONO"].ToString().Trim() + "%' ";
			sqlstr += "UNION SELECT * FROM TPSSM41  WHERE 1 = 1";
			if (s_factory_div != "") sqlstr += " AND FACTORY_DIV = '" + s_factory_div + "' ";   //2023.1.16
			if (tpssm11["HEAT_NO"].ToString().Trim() != "")	sqlstr += " AND HEAT_NO like '%" + tpssm11["HEAT_NO"].ToString().Trim() + "%' ";
			if (tpssm11["PONO"].ToString().Trim() != "")		sqlstr += " AND PONO    like '%" + tpssm11["PONO"].ToString().Trim() + "%' ";
			sqlstr += ") where heat_no != ' ' ORDER BY CAST_NO ASC, CAST_DIV_NO ASC ";
			break;
		}

		Log::Trace("", "", "sqlstr=[{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();

		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tpssm11);

			cast_no_show = tpssm11["CAST_NO"].ToString() + "-" + tpssm11["CAST_DIV_NO"].ToDecimal().ToString();

			Log::Trace("", "", "cast_no_show=[{0}]", (const char*)cast_no_show);
			Log::Trace("", "", "heat_no=[{0}]", (const char*)tpssm11["HEAT_NO"].ToString());

			//查询计划状态
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT MAX(PONO_STATUS) "
					"  FROM (SELECT PONO_STATUS FROM TPSSM11 WHERE HEAT_NO = @heat_no "
					"         UNION "
					"        SELECT PONO_STATUS FROM TPSSM41 WHERE HEAT_NO = @heat_no) ";
				break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("heat_no", tpssm11["HEAT_NO"].ToString().Trim());
			cmd_inq_01.ExecuteReader();
			pono_status = cmd_inq_01.ExecuteScalar();
			cmd_inq_01.Close();

			/*******增加牌号*******/  //update by liyongle 20171127
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT LABEL1 "
					"  FROM TQMTS0X WHERE ST_NO = @st_no ";
				break;
			}
			cmd_inq_02.SetCommandText(sqlstr);
			cmd_inq_02.Parameters.Set("st_no", tpssm11["ST_NO"].ToString().Trim());
			cmd_inq_02.ExecuteReader();
			if (cmd_inq_02.Read())
			{
				sg_sign = cmd_inq_02.GetString(1);
			}
			cmd_inq_02.Close();

			CDataRow& row = bcls_ret->Tables[0].Rows.Add();   //新增空行
			row["HEAT_NO"] = tpssm11["HEAT_NO"];
			row["PONO"] = tpssm11["PONO"];
			row["SM_PLAN_NO"] = tpssm11["SM_PLAN_NO"];
			row["ST_NO"] = tpssm11["ST_NO"];
			row["REFINE_ROUTE_CODE"] = tpssm11["REFINE_ROUTE_CODE"];
			row["CAST_NO_SHOW"] = cast_no_show;
			row["PONO_STATUS"] = pono_status;
			row["FACTORY_DIV"] = tpssm11["FACTORY_DIV"];
			row["SG_SIGN"] = sg_sign;

			s_rep_elm_sel_flag = "";
			//查询代表成分标记
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT REP_ELM_SEL_FLAG "
					"  FROM TQMTS24 WHERE HEAT_NO = @heat_no and REP_ELM_SEL_FLAG !=' ' ";
				break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("heat_no", tpssm11["HEAT_NO"].ToString().Trim());
			cmd_inq_01.ExecuteReader();
			if (cmd_inq_01.Read())
			{
				s_rep_elm_sel_flag = cmd_inq_01.GetString(1);
			}
			cmd_inq_01.Close();
			row["REP_ELM_SEL_FLAG"] = s_rep_elm_sel_flag;
		}
		cmd_inq.Close();


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
