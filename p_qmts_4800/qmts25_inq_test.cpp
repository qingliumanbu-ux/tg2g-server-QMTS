/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-13
Description: 成分实绩查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
/// 成分实绩查询
/// <para>
/// 1.成分实绩查询。
///
/// </para>
/// <para>数据库表：TQMTS25(实绩_工序成分)/TQMTS24(实绩_成分主信息)		</para>
/// <para>主调用函数：前台QMTS25画面F2查询按钮					                </para>
/// <para>需调用函数：							</para>
/// </summary>
/// <param name="heat_no">熔炼号				</param>
/// <param name="st_sample_no">试样号			</param>
/// <returns>  </returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts25_inq_test)

int f_qmts25_inq_test(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	int q_flag = 0;
	CString q_heat_no = " ";
	CString q_st_no = " ";
	CString q_whole_backlog_code = " ";
	CString q_st_sample_div = " ";
	CString q_gas_type_div = " ";
	CString q_analyse_time_from = " ";
	CString q_analyse_time_to = " ";
	int i_every_page = 50;
	int i_now_record = 1;

	int RecordFrom = 0;
	int PageSize = 0;
	CDecimal row_id = 0;
	CDecimal i_total_count = 0;
	CString s_station_no = "";
	CString s_station_name = "";
	int count = 0;

	CString sqlstr = "";

	CString s_factory_div = "";  //2023.1.16

	/* 实体类定义 */
	CModel tqmts25("TQMTS25");
	CModel tep0002("TEP0002");
	CModel tqmts24("TQMTS24");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);
	CDbCommand cmd_inq_tep0002(conn);

	try
	{
	

		q_flag = bcls_rec->Tables[0].Rows[0]["flag"].ToDecimal().ToInt16();
		q_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		q_whole_backlog_code = bcls_rec->Tables[0].Rows[0]["whole_backlog_code"].ToString().Trim();
		q_st_sample_div = bcls_rec->Tables[0].Rows[0]["st_sample_div"].ToString().Trim();
		q_gas_type_div = bcls_rec->Tables[0].Rows[0]["gas_type_div"].ToString().Trim();
		q_analyse_time_from = bcls_rec->Tables[0].Rows[0]["analyse_time_from"].ToString().Trim();
		q_analyse_time_to = bcls_rec->Tables[0].Rows[0]["analyse_time_to"].ToString().Trim();

		//if (bcls_rec->Tables[0].Columns.Contains("every_page"))
		//	i_every_page = bcls_rec->Tables["PageInfo"].Rows[0]["every_page"].ToDecimal().ToInt32();/*每页显示的记录数*/
		//if (bcls_rec->Tables[0].Columns.Contains("now_record"))
		//	i_now_record = bcls_rec->Tables["PageInfo"].Rows[0]["now_record"].ToDecimal().ToInt32();/*开始记录序号 i_every_page*PageFrom*/
		if (q_flag != 0)   //初始化列
		{
			RecordFrom = bcls_rec->Tables["PageInfo"].Rows[0]["RecordFrom"].ToDecimal().ToInt32();
			PageSize = bcls_rec->Tables["PageInfo"].Rows[0]["PageSize"].ToDecimal().ToInt32();
		}
			
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV")) s_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();

		Log::Trace("", "", "qmts25_inq IN:---q_flag = [{0}]", q_flag);
		Log::Trace("", "", "qmts25_inq IN:---q_heat_no = [{0}]", (const char*)q_heat_no);
		Log::Trace("", "", "qmts25_inq IN:---q_st_no = [{0}]", (const char*)q_st_no);
		Log::Trace("", "", "qmts25_inq IN:---q_whole_backlog_code = [{0}]", (const char*)q_whole_backlog_code);
		Log::Trace("", "", "qmts25_inq IN:---q_st_sample_div = [{0}]", (const char*)q_st_sample_div);
		Log::Trace("", "", "qmts25_inq IN:---q_gas_type_div = [{0}]", (const char*)q_gas_type_div);
		Log::Trace("", "", "qmts25_inq IN:---q_analyse_time_from[{0}], q_analyse_time_to[{1}]", (const char*)q_analyse_time_from, (const char*)q_analyse_time_to);
		Log::Trace("", "", "qmts25_inq IN:---i_every_page[{0}], i_now_record[{1}]", i_every_page, i_now_record); Log::Trace("", "", "qmts25_inq IN:---q_flag = [{0}]", q_flag);
		Log::Trace("", "", "qmts25_inq IN:---FACTORY_DIV = [{0}]", s_factory_div);
		Log::Trace("", "", "qmts25_inq IN:---RecordFrom = [{0}]", RecordFrom);
		Log::Trace("", "", "qmts25_inq IN:---PageSize = [{0}]", PageSize);

		//设置输出块列名
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		bcls_ret->Tables[0].Columns["HEAT_NO"].set_Caption(_RES("QM00S0004301")/*熔炼号*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "PONO");
		bcls_ret->Tables[0].Columns["PONO"].set_Caption(_RES("QM00S0004303")/*制造命令号*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
		bcls_ret->Tables[0].Columns["ST_SAMPLE_NO"].set_Caption(_RES("QM00S0004304")/*试样号*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_NO");
		//bcls_ret->Tables[0].Columns["ST_NO"].set_Caption("出钢记号");
		bcls_ret->Tables[0].Columns["ST_NO"].set_Caption("出钢记号");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "STATION_NAME");
		bcls_ret->Tables[0].Columns["STATION_NAME"].set_Caption("工位");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_DIV");
		bcls_ret->Tables[0].Columns["ST_SAMPLE_DIV"].set_Caption("试样区分");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "GAS_TYPE_DIV");
		bcls_ret->Tables[0].Columns["GAS_TYPE_DIV"].set_Caption("气体区分");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "JUDGE_CODE");
		bcls_ret->Tables[0].Columns["JUDGE_CODE"].set_Caption("判定结果代码");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "JUDGE_DESC");
		bcls_ret->Tables[0].Columns["JUDGE_DESC"].set_Caption("判定结果");

		/****************************************************************/
		/*******************根据代码配置表压入列名***********************/
		/****************************************************************/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TEP0002 "
				" WHERE CODE_CLASS = 'QMYS' "
				"   AND CODE_DESC_2_CONTENT !=' ' "
				" ORDER BY CODE_DESC_2_CONTENT ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tep0002);
			Log::Trace("", "", "tep0002.CODE[{0}], tep0002.CODE_DESC_1_CONTENT[{1}]", (const char*)tep0002["CODE"].ToString(), (const char*)tep0002["CODE_DESC_1_CONTENT"].ToString());
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "H5_"+tep0002["CODE"].ToString());  //压入列名(元素代码) 
			bcls_ret->Tables[0].Columns["H5_"+tep0002["CODE"].ToString()].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString());//压入列标题(元素名)
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "H5_"+tep0002["CODE"].ToString() + "_OK");
			bcls_ret->Tables[0].Columns["H5_" + tep0002["CODE"].ToString() + "_OK"].set_Caption(tep0002["CODE_DESC_1_CONTENT"].ToString() + "判定结果");//压入列标题(元素名)
		}
		cmd_inq.Close();
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NO");
		bcls_ret->Tables[0].Columns["SM_PLAN_NO"].set_Caption(_RES("QM00S0004308")/*炼钢计划号*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "C_S_JUDGE_DIV");
		bcls_ret->Tables[0].Columns["C_S_JUDGE_DIV"].set_Caption(_RES("QM00S0004332")/*C/S参考区分*/);

	//	bcls_ret->Tables[0].Columns.Add(DT_DATETIME, "ANALYSE_TIME");
		//bcls_ret->Tables[0].Columns["ANALYSE_TIME"].set_Caption(_RES("QM00S0004307")/*分析时刻*/);
		//bcls_ret->Tables[0].Columns.Add(DT_DATETIME, "REC_CREATE_TIME");
		//bcls_ret->Tables[0].Columns["REC_CREATE_TIME"].set_Caption(_RES("QM00S0004193")/*记录创建时刻*/);
		//bcls_ret->Tables[0].Columns.Add(DT_DATETIME, "REC_REVISE_TIME");
		//bcls_ret->Tables[0].Columns["REC_REVISE_TIME"].set_Caption(_RES("QM00S0004195")/*记录修改时刻*/);


		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SAMPLE_IFGOOD");
		bcls_ret->Tables[0].Columns["SAMPLE_IFGOOD"].set_Caption(_RES("QM00S0004333")/*试样良否*/);

		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ANALYSE_TIM");
		bcls_ret->Tables[0].Columns["ANALYSE_TIM"].set_Caption(_RES("QM00S0004307")/*分析时刻*/);
		
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REC_CREATE_TIM");  
		bcls_ret->Tables[0].Columns["REC_CREATE_TIM"].set_Caption(_RES("QM00S0004193")/*记录创建时刻*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REC_CREATOR");
		bcls_ret->Tables[0].Columns["REC_CREATOR"].set_Caption(_RES("QM00S0004192")/*记录创建责任者*/);
		
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REC_REVISE_TIM");
		bcls_ret->Tables[0].Columns["REC_REVISE_TIM"].set_Caption(_RES("QM00S0004195")/*记录修改时刻*/);
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REC_REVISOR");
		bcls_ret->Tables[0].Columns["REC_REVISOR"].set_Caption(_RES("QM00S0004194")/*记录修改责任者*/);

		
		
	

		//flag=0,画面load元素信息用，不需取值，跳出
		if (q_flag == 0)
		{
			bcls_ret->Tables[0].Rows.Add();   //不加一行前台报错
			return doFlag;
		}

		Log::Trace("", "", "SELECT TQMTS24");
		/****************************************************************/
		/*******************从实绩表中读出元素值*************************/
		/****************************************************************/
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			Log::Trace("", "", "走DB2");
			sqlstr = " SELECT * "
				"   FROM (SELECT ROW_NUMBER() over() as ROW_ID,A.* "
				"           FROM (SELECT DISTINCT FACTORY_DIV,HEAT_NO,PONO,ST_SAMPLE_NO,ST_NO,WHOLE_BACKLOG_CODE,ST_SAMPLE_DIV,GAS_TYPE_DIV,SM_PLAN_NO,C_S_JUDGE_DIV,ANALYSE_TIME,SAMPLE_IFGOOD,JUDGE_CODE "
				"                   FROM TQMTS24 "
				"                  WHERE ST_SAMPLE_DIV != '3' ";
			if (s_factory_div != "") sqlstr += " AND FACTORY_DIV = '" + s_factory_div + "' ";
			if (q_heat_no.Trim() != "")
				sqlstr += "                    AND HEAT_NO LIKE @q_heat_no||'%' ";
			if (q_st_no.Trim() != "")
				sqlstr += "                    AND ST_NO LIKE @q_st_no||'%' ";
			if (q_whole_backlog_code.Trim() != "")
				sqlstr += "                    AND WHOLE_BACKLOG_CODE = @q_whole_backlog_code ";
			if (q_st_sample_div.Trim() != "")
				sqlstr += "                    AND ST_SAMPLE_DIV = @q_st_sample_div  ";
			if (q_gas_type_div.Trim() != "")
				sqlstr += "                    AND GAS_TYPE_DIV = @q_gas_type_div  ";
			if (q_analyse_time_from.Trim() != "")  sqlstr = sqlstr + " AND ANALYSE_TIME>= @q_analyse_time_from ";
			if (q_analyse_time_to.Trim() != "")  sqlstr = sqlstr + " AND ANALYSE_TIME <= @q_analyse_time_to ";
			sqlstr = sqlstr + "                  ORDER BY HEAT_NO ASC, ST_SAMPLE_NO ASC "
				"                ) A "
				"        ) "
			//	"  WHERE ROW_ID > @i_now_record AND ROW_ID <= (@i_now_record + @i_every_page) ";  //原始版本
				"  WHERE ROW_ID >= @RecordFrom AND ROW_ID <= (@RecordFrom + @PageSize-1) ";
			break;   
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM (SELECT ROWNUM ROW_ID,A.* "
				"           FROM (SELECT DISTINCT FACTORY_DIV,HEAT_NO,PONO,ST_SAMPLE_NO,ST_NO,WHOLE_BACKLOG_CODE,ST_SAMPLE_DIV,GAS_TYPE_DIV,SM_PLAN_NO,C_S_JUDGE_DIV,ANALYSE_TIME,SAMPLE_IFGOOD,JUDGE_CODE "
				"                   FROM TQMTS24 "
				"                  WHERE ST_SAMPLE_DIV != '3' ";   
		/*	sqlstr = "SELECT DISTINCT FACTORY_DIV,HEAT_NO,PONO,ST_SAMPLE_NO,ST_NO,WHOLE_BACKLOG_CODE,ST_SAMPLE_DIV,GAS_TYPE_DIV,SM_PLAN_NO,C_S_JUDGE_DIV,ANALYSE_TIME,SAMPLE_IFGOOD,JUDGE_CODE"
			"	FROM TQMTS24 WHERE ST_SAMPLE_DIV != '3'";   */    //没有分页版本
			if (s_factory_div != "") sqlstr += " AND FACTORY_DIV = '" + s_factory_div + "' ";
			if (q_heat_no.Trim() != "")
				sqlstr += "                    AND HEAT_NO LIKE @q_heat_no||'%' ";
			if (q_st_no.Trim() != "")
				sqlstr += "                    AND ST_NO LIKE @q_st_no||'%' ";
			if (q_whole_backlog_code.Trim() != "")
				sqlstr += "                    AND WHOLE_BACKLOG_CODE = @q_whole_backlog_code ";
			if (q_st_sample_div.Trim() != "")
				sqlstr += "                    AND ST_SAMPLE_DIV = @q_st_sample_div  ";
			if (q_gas_type_div.Trim() != "")
				sqlstr += "                    AND GAS_TYPE_DIV = @q_gas_type_div  ";
			if (q_analyse_time_from.Trim() != "")  sqlstr = sqlstr + " AND ANALYSE_TIME>= @q_analyse_time_from ";
			if (q_analyse_time_to.Trim() != "")  sqlstr = sqlstr + " AND ANALYSE_TIME <= @q_analyse_time_to ";
			sqlstr = sqlstr + "                  ORDER BY HEAT_NO ASC, ST_SAMPLE_NO ASC "
				"                ) A "
				"        ) "
				//	"  WHERE ROW_ID > @i_now_record AND ROW_ID <= (@i_now_record + @i_every_page) ";  //原始版本
				"  WHERE ROW_ID >= @RecordFrom AND ROW_ID <= (@RecordFrom + @PageSize-1) ";
			break;
		}
		Log::Trace("", "", "qmts24sqlstr = [{0}]", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("q_heat_no", q_heat_no.Trim());
		cmd_inq.Parameters.Set("q_st_no", q_st_no.Trim());
		cmd_inq.Parameters.Set("q_whole_backlog_code", q_whole_backlog_code.Trim());
		cmd_inq.Parameters.Set("q_st_sample_div", q_st_sample_div.Trim());
		cmd_inq.Parameters.Set("q_gas_type_div", q_gas_type_div.Trim());
		cmd_inq.Parameters.Set("q_analyse_time_from", q_analyse_time_from.Trim());
		cmd_inq.Parameters.Set("q_analyse_time_to", q_analyse_time_to.Trim());
	/*	cmd_inq.Parameters.Set("i_now_record", i_now_record);
		cmd_inq.Parameters.Set("i_every_page", i_every_page);*/
		cmd_inq.Parameters.Set("RecordFrom", RecordFrom);
		cmd_inq.Parameters.Set("PageSize", PageSize);
		cmd_inq.ExecuteReader();
		Log::Trace("", "", "循环处理数据");
		while (cmd_inq.Read())
		{
			/*count++;
			if (count >= 50)
				break;*/
			
			//row_id = cmd_inq.GetDecimal(1);
			tqmts24["FACTORY_DIV"] = cmd_inq.GetString(2);
			tqmts24["HEAT_NO"] = cmd_inq.GetString(3);
			tqmts24["PONO"] = cmd_inq.GetString(4);
			tqmts24["ST_SAMPLE_NO"] = cmd_inq.GetString(5);
			tqmts24["ST_NO"] = cmd_inq.GetString(6);
			tqmts24["WHOLE_BACKLOG_CODE"] = cmd_inq.GetString(7);
			tqmts24["ST_SAMPLE_DIV"] = cmd_inq.GetString(8);
			tqmts24["GAS_TYPE_DIV"] = cmd_inq.GetString(9);
			tqmts24["SM_PLAN_NO"] = cmd_inq.GetString(10);
			tqmts24["C_S_JUDGE_DIV"] = cmd_inq.GetString(11);
			tqmts24["ANALYSE_TIME"] = cmd_inq.GetString(12);
			tqmts24["SAMPLE_IFGOOD"] = cmd_inq.GetString(13);
			tqmts24["JUDGE_CODE"] = cmd_inq.GetString(14);

			Log::Trace("", "", "tqmts24.HEAT_NO = [{0}]", (const char*)tqmts24["HEAT_NO"].ToString());
			Log::Trace("", "", "tqmts24.ST_SAMPLE_NO = [{0}]", (const char*)tqmts24["ST_SAMPLE_NO"].ToString());
			Log::Trace("", "", "tqmts24.ANALYSE_TIME = [{0}]", (const char*)tqmts24["ANALYSE_TIME"].ToString());
			//HYF 20130427 SubstringNE
			s_station_no = tqmts24["ST_SAMPLE_NO"].ToString().SubstringNE(3, 1);

			Log::Trace("", "", "FACTORY_DIV = [{0}]", (const char*)tqmts24["FACTORY_DIV"].ToString());
			Log::Trace("", "", "STATION_ID = [{0}]", (const char*)tqmts24["WHOLE_BACKLOG_CODE"].ToString());
			Log::Trace("", "", "STATION_NO = [{0}]", (const char*)s_station_no);

			s_station_name = "";
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT STATION_NAME "
					"  FROM TPSSMD1 "
					" WHERE SUBSTR(FACTORY_DIV,1,1) = @tqmts24.FACTORY_DIV "
					"   AND STATION_ID = @tqmts24.WHOLE_BACKLOG_CODE "
					"   AND STATION_NO = @s_station_no ";
				break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("tqmts24.FACTORY_DIV", tqmts24["FACTORY_DIV"].ToString().Trim());
			cmd_inq_01.Parameters.Set("tqmts24.WHOLE_BACKLOG_CODE", tqmts24["WHOLE_BACKLOG_CODE"].ToString().Trim());
			cmd_inq_01.Parameters.Set("s_station_no", s_station_no.Trim());
			cmd_inq_01.ExecuteReader();
			if (cmd_inq_01.Read())
			{
				s_station_name = cmd_inq_01.GetString(1);
			}
			cmd_inq_01.Close();
			Log::Trace("", "", "STATION_NAME = [{0}]", (const char*)s_station_name);

			//当页的记录返回前台
			CDataRow& row = bcls_ret->Tables[0].Rows.Add();
			row["HEAT_NO"] = tqmts24["HEAT_NO"];
			row["PONO"] = tqmts24["PONO"];
			row["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
			row["ST_NO"] = tqmts24["ST_NO"];
			row["STATION_NAME"] = s_station_name;
			row["ST_SAMPLE_DIV"] = tqmts24["ST_SAMPLE_DIV"];
			row["GAS_TYPE_DIV"] = tqmts24["GAS_TYPE_DIV"];
			row["SM_PLAN_NO"] = tqmts24["SM_PLAN_NO"];
			row["C_S_JUDGE_DIV"] = tqmts24["C_S_JUDGE_DIV"];
			if (tqmts24["ANALYSE_TIME"].ToString().TrimOrBlank() != " ")
				row["ANALYSE_TIM"] = tqmts24["ANALYSE_TIME"].ToString().Substring(0, 4) + "-" + tqmts24["ANALYSE_TIME"].ToString().Substring(4, 2) + "-" + tqmts24["ANALYSE_TIME"].ToString().Substring(6, 2) + " "
				+ tqmts24["ANALYSE_TIME"].ToString().Substring(8, 2) + ":" + tqmts24["ANALYSE_TIME"].ToString().Substring(10, 2) + ":" + tqmts24["ANALYSE_TIME"].ToString().Substring(12, 2);
		//	row["ANALYSE_TIME"] = (const char*)tqmts24["ANALYSE_TIME"].ToString();
			Log::Trace("", "", "row ANALYSE_TIM  = [{0}]", row["ANALYSE_TIM"].ToString());
			row["SAMPLE_IFGOOD"] = tqmts24["SAMPLE_IFGOOD"];
			row["JUDGE_CODE"] = tqmts24["JUDGE_CODE"].ToString();
			Log::Trace("", "", "tqmts24 JUDGE_CODE  = [{0}]", tqmts24["JUDGE_CODE"].ToString());
			//查询判定结果描述
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TEP0002 "
					" WHERE CODE_CLASS = 'QMQ6' "
					"   AND CODE = @tqmts24.JUDGE_CODE "
					;
				break;
			}
			cmd_inq_tep0002.SetCommandText(sqlstr);
			cmd_inq_tep0002.Parameters.Set("tqmts24.JUDGE_CODE", tqmts24["JUDGE_CODE"].ToString());
			cmd_inq_tep0002.ExecuteReader();
			if (cmd_inq_tep0002.Read())
			{
				cmd_inq_tep0002.Fetch(tep0002);
				row["JUDGE_DESC"] = tep0002["CODE_DESC_1_CONTENT"];
			}
			else
			{
				row["JUDGE_DESC"] = " ";
			}
			cmd_inq_tep0002.Close();
			Log::Trace("", "", "row JUDGE_DESC  = [{0}]", row["JUDGE_DESC"]);

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TQMTS25 "
					" WHERE HEAT_NO = @tqmts24.HEAT_NO "
					"   AND ST_SAMPLE_NO = @tqmts24.ST_SAMPLE_NO "
					" ORDER BY ELM_CODE ASC ";
				break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("tqmts24.HEAT_NO", tqmts24["HEAT_NO"].ToString().Trim());
			cmd_inq_01.Parameters.Set("tqmts24.ST_SAMPLE_NO", tqmts24["ST_SAMPLE_NO"].ToString().Trim());
			cmd_inq_01.ExecuteReader();
			while (cmd_inq_01.Read())
			{
				cmd_inq_01.Fetch(tqmts25);
				Log::Trace("", "", "tqmts25.ELM_CODE[{0}], tqmts25.ELM_ACT[{1}], tqmts25.ELM_OK[{2}]", (const char*)tqmts25["ELM_CODE"].ToString(), tqmts25["ELM_ACT"].ToDecimal().ToDouble(), tqmts25["ELM_OK"].ToDecimal().ToInt32());
				row["H5_"+tqmts25["ELM_CODE"].ToString()] = tqmts25["ELM_ACT"];
				row["H5_"+tqmts25["ELM_CODE"].ToString() + "_OK"] = tqmts25["ELM_OK"];
			}
			cmd_inq_01.Close();

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME "
					"  FROM TQMTS24 "
					" WHERE HEAT_NO = @tqmts24.HEAT_NO "
					"   AND ST_SAMPLE_NO = @tqmts24.ST_SAMPLE_NO ";
				break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("tqmts24.HEAT_NO", tqmts24["HEAT_NO"].ToString().Trim());
			cmd_inq_01.Parameters.Set("tqmts24.ST_SAMPLE_NO", tqmts24["ST_SAMPLE_NO"].ToString().Trim());
			cmd_inq_01.ExecuteReader();
			if (cmd_inq_01.Read())
			{
				tqmts24["REC_CREATOR"] = cmd_inq_01.GetString(1);
				tqmts24["REC_CREATE_TIME"] = cmd_inq_01.GetString(2);
				tqmts24["REC_REVISOR"] = cmd_inq_01.GetString(3);
				tqmts24["REC_REVISE_TIME"] = cmd_inq_01.GetString(4);
			}
			cmd_inq_01.Close();
			Log::Trace("", "", "tqmts24.REC_CREATE_TIME = [{0}]", (const char*)tqmts24["REC_CREATE_TIME"].ToString());
			row["REC_CREATOR"] = tqmts24["REC_CREATOR"];
			if (tqmts24["REC_CREATE_TIME"].ToString().TrimOrBlank() != " ")  //h5动态列这里带有time字段的列前台确实不好控制,暂时在后台拼
				//	row["REC_CREATE_TIME"] = CDateTime::Parse(tqmts24["REC_CREATE_TIME"].ToString());
				row["REC_CREATE_TIM"] = tqmts24["REC_CREATE_TIME"].ToString().Substring(0, 4) + "-" + tqmts24["REC_CREATE_TIME"].ToString().Substring(4, 2) + "-" + tqmts24["REC_CREATE_TIME"].ToString().Substring(6, 2) + " "
				+ tqmts24["REC_CREATE_TIME"].ToString().Substring(8, 2) + ":" + tqmts24["REC_CREATE_TIME"].ToString().Substring(10, 2) + ":" + tqmts24["REC_CREATE_TIME"].ToString().Substring(12, 2);
			
			row["REC_REVISOR"] = tqmts24["REC_REVISOR"];
			if (tqmts24["REC_REVISE_TIME"].ToString().TrimOrBlank() != " ")
				//	row["REC_REVISE_TIME"] = CDateTime::Parse(tqmts24["REC_REVISE_TIME"].ToString());
				row["REC_REVISE_TIM"] = tqmts24["REC_REVISE_TIME"].ToString().Substring(0, 4) + "-" + tqmts24["REC_REVISE_TIME"].ToString().Substring(4, 2) + "-" + tqmts24["REC_REVISE_TIME"].ToString().Substring(6, 2) + " "
				+ tqmts24["REC_REVISE_TIME"].ToString().Substring(8, 2) + ":" + tqmts24["REC_REVISE_TIME"].ToString().Substring(10, 2) + ":" + tqmts24["REC_REVISE_TIME"].ToString().Substring(12, 2);
		}
		cmd_inq.Close();

		bcls_ret->Tables.Add();
		bcls_ret->Tables[1].Columns.Add(DT_DECIMAL, "TOTAL_COUNT");
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT COUNT(1) "
				"  FROM (SELECT DISTINCT FACTORY_DIV,HEAT_NO,PONO,ST_SAMPLE_NO,ST_NO,WHOLE_BACKLOG_CODE,ST_SAMPLE_DIV,GAS_TYPE_DIV,SM_PLAN_NO,C_S_JUDGE_DIV,ANALYSE_TIME,SAMPLE_IFGOOD "
				"          FROM TQMTS24 "
				"                  WHERE ST_SAMPLE_DIV != '3' ";
			if (q_heat_no.Trim() != "")
				sqlstr += "                    AND HEAT_NO LIKE @q_heat_no||'%' ";
			if (q_st_no.Trim() != "")
				sqlstr += "                    AND ST_NO LIKE @q_st_no||'%' ";
			if (q_whole_backlog_code.Trim() != "")
				sqlstr += "                    AND WHOLE_BACKLOG_CODE = @q_whole_backlog_code ";
			if (q_st_sample_div.Trim() != "")
				sqlstr += "                    AND ST_SAMPLE_DIV = @q_st_sample_div  ";
			if (q_gas_type_div.Trim() != "")
				sqlstr += "                    AND GAS_TYPE_DIV = @q_gas_type_div  ";
			if (q_analyse_time_from.Trim() != "")  sqlstr = sqlstr + " AND ANALYSE_TIME>= @q_analyse_time_from ";
			if (q_analyse_time_to.Trim() != "")  sqlstr = sqlstr + " AND ANALYSE_TIME <= @q_analyse_time_to ";
			sqlstr = sqlstr + "       ) ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("q_heat_no", q_heat_no.Trim());
		cmd_inq.Parameters.Set("q_st_no", q_st_no.Trim());
		cmd_inq.Parameters.Set("q_whole_backlog_code", q_whole_backlog_code.Trim());
		cmd_inq.Parameters.Set("q_st_sample_div", q_st_sample_div.Trim());
		cmd_inq.Parameters.Set("q_gas_type_div", q_gas_type_div.Trim());
		cmd_inq.Parameters.Set("q_analyse_time_from", q_analyse_time_from.Trim());
		cmd_inq.Parameters.Set("q_analyse_time_to", q_analyse_time_to.Trim());
		i_total_count = cmd_inq.ExecuteScalar();
		Log::Trace("", "", "total_count = [{0}]", i_total_count);
		cmd_inq.Close();
		CDataRow& row = bcls_ret->Tables[1].Rows.Add();
		bcls_ret->Tables[1].Rows[0]["TOTAL_COUNT"] = i_total_count;
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
