/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      
Version:     1.0
Date:        2023-05-24 14:48:51
Description: 炉次判定-炉次信息查询   
热检信息：TQMTS23   TPSSM13
冷检信息：TQMTS23   TPSSM41
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmts21m_inq_23)


int f_qmts21m_inq_23(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sql = " ";
	CModel tpssm10("TPSSM10");
	CModel tqmts0x("TQMTS0X");
	try
	{
		int	record_count_per_page = 0;		/* 每页记录数 */
		int	current_page_no = 0;			/* 需查询的页号,从0开始计数 */
		int	start_row = 0;					/* 将要压入outBlock的起始行 */
		Log::Trace("", "", "qmts21m_23_inq 开始获取参数");
		/* 获取传入的表名 */
		/*CString sql_orderBy = (CString)bcls_rec->Tables[1].Rows[0]["ORDER_BY"].ToString().Trim();
		Log::Trace("", "", "qmts21m_23_inq IN: sql_orderBy[{0}]", (const char*)sql_orderBy);*/
		/* 每页记录数 */
		record_count_per_page = bcls_rec->Tables[1].Rows[0]["PAGE_SIZE"];
		Log::Trace("", "", "qmts21m_23_inq IN: record_count_per_page[{0}]", record_count_per_page);
		/* 需查询的页号 */
		current_page_no = bcls_rec->Tables[1].Rows[0]["PAGE_NUM"];
		Log::Trace("", "", "qmts21m_23_inq IN: current_page_no[{0}]", current_page_no);

		/*筛选条件*/
		CString pono = bcls_rec->Tables[0].Rows[0]["pono"].ToString().Trim();
		CString heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		CString cc_mach_no = bcls_rec->Tables[0].Rows[0]["cc_mach_no"].ToString().Trim();
		CString table_name = bcls_rec->Tables[0].Rows[0]["table_name"].ToString().Trim();
		Log::Trace("", "", "qmts21m_23_inq IN: pono[{0}]heat_no[{1}]cc_mach_no[{2}]", (const char*)pono, (const char*)heat_no, (const char*)cc_mach_no);
		CString heat_confm_time_from = "";
		CString heat_confm_time_to = "";
		CString confm_flag = "";
		if (bcls_rec->Tables[0].Columns.Contains("heat_confm_time_from"))
		{
			heat_confm_time_from = bcls_rec->Tables[0].Rows[0]["heat_confm_time_from"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("heat_confm_time_to"))
		{
			heat_confm_time_to = bcls_rec->Tables[0].Rows[0]["heat_confm_time_to"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("CONFM_FLAG"))
		{
			confm_flag = bcls_rec->Tables[0].Rows[0]["CONFM_FLAG"].ToString().TrimOrBlank();
		}
		Log::Trace("", "", "qmts21m_23_inq IN: confm_flag[{0}]", confm_flag);
		Log::Trace("", "", "qmts21m_23_inq IN: heat_confm_time_from[{0}]", heat_confm_time_from);
		Log::Trace("", "", "qmts21m_23_inq IN: heat_confm_time_to[{0}]", heat_confm_time_to);
		CDbCommand cmd(conn);
		//炉次主信息
		switch (conn->DatabaseKind)
		{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sql = " SELECT A.* ,B.*   FROM " + table_name + " A ";
				break;
		}

		/*拼接查询sql条数语句*/
		CString sql_count = "SELECT COUNT(*) FROM " + table_name + " A ";

		/*拼接查询sql语句*/
		CString sql_where = " ";
		if (table_name == "TPSSM41")
		{
			sql_where = sql_where + " LEFT JOIN TQMTS23 B ON A.HEAT_NO = B.HEAT_NO AND  DECI_ST_NO  =  'YY000000' AND  FIN_ST_NO = ' ' WHERE 1 =1 ";
		}
		else
		{
			sql_where = " LEFT JOIN TQMTS23 B ON A.HEAT_NO = B.HEAT_NO WHERE 1=1  AND RUN_STATUS >='51' ";
		}

		//CString sql_where = " WHERE 1=1  AND  A.PONO_STATUS >= 51 ";
		

		//查询条件
		if (pono.Trim() != "")
		{
			sql_where = sql_where + "  AND A.pono LIKE '%'||@pono||'%' ";
			cmd.Parameters.Set("pono", pono.Trim());
		}

		if (heat_no.Trim() != "")
		{
			sql_where = sql_where + "  AND A.heat_no LIKE '%'||@heat_no||'%' ";
			cmd.Parameters.Set("heat_no", heat_no.Trim());
		}

		if (cc_mach_no.Trim() != "")
		{
			sql_where = sql_where + "  AND A.cc_mach_no LIKE '%'||@cc_mach_no||'%' ";
			cmd.Parameters.Set("cc_mach_no", cc_mach_no.Trim());
		}
		
		if (confm_flag.Trim()=="1" && table_name=="TPSSM41")
		{
			if (heat_confm_time_from.GetLength()>0)
			{
				sql_where = sql_where + "  AND A.heat_confm_time >=@heat_confm_time_from ";
				cmd.Parameters.Set("heat_confm_time_from", heat_confm_time_from + "000000");
			}

			if (heat_confm_time_to.GetLength()>0)
			{
				sql_where = sql_where + "  AND A.heat_confm_time <=@heat_confm_time_to ";
				cmd.Parameters.Set("heat_confm_time_from", heat_confm_time_from + "235959");
			}
			
		}

		

		/*连接sql语句*/
		sqlstr = sql_count + sql_where;
		Log::Trace("", "", "qmts21m_23_inq 统计SQL[{0}]", sqlstr);
		cmd.SetCommandText(sqlstr);

		/*获取条数*/
		CDecimal rc = cmd.ExecuteScalar();

		/*把值压入RC中，传出前台*/
		bcls_ret->ExtendedProperties.Add("RC", rc.ToString());

		
		CString sql_orderBy = " ORDER BY A.CC_REQ_TIME ASC " ;
		
		/*完成拼接查询sql*/
		sqlstr = sql + sql_where + sql_orderBy;
		Log::Trace("", "", "qmts21m_23_inq查询sql[{0}],", sqlstr);
		cmd.SetCommandText(sqlstr);

		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > rc.ToDouble())
		{
			start_row = 0;
		}
		Log::Trace("", __FUNCTION__, "current_page_no		= [{0}]", current_page_no);
		Log::Trace("", __FUNCTION__, "record_count_per_page = [{0}]", record_count_per_page);
		Log::Trace("", __FUNCTION__, "start_row			    = [{0}]", start_row);

		int count = cmd.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
		//增加单独返回列,查询工艺卡等信息
		CDbCommand cmd1(conn);
		//bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ABNY_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "ABN_CONT");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "PURE_DEGAS_DURATION");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "O2_SUM_COMSUME");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REFINE_ROUTE_CODE1");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SLAB_CHECK_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "SLAB_FINISH_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "O_GAS_DIV");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "H_GAS_DIV");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "N_GAS_DIV");
		CString ST_SAMPLE_ENTR_NO = " ";
		CString ST_SAMPLE_NO = " ";
		CString ABNY_CODE = " ";
		CString ABN_CONT = " ";
		CDecimal PURE_DEGAS_DURATION = 0;
		CDecimal O2_SUM_COMSUME = 0;
		for (int i = 0; i < count; i++)
		{
			tpssm10["PONO"] = bcls_ret->Tables[0].Rows[i]["PONO"].ToString();
			tpssm10["ST_NO"] = bcls_ret->Tables[0].Rows[i]["ST_NO"].ToString();

			cmd1.SetCommandText("SELECT  ST_SAMPLE_NO	 FROM TQMTS24 "
				 " WHERE  PONO = @pono "
				 "  AND WHOLE_BACKLOG_CODE = 'C' "
				 " AND REP_ELM_SEL_FLAG = '1' "
				 "  FETCH FIRST 1 ROWS ONLY ");
			cmd1.Parameters.Set("pono", tpssm10["PONO"]);
			cmd1.ExecuteReader();
			if (cmd1.Read())
			{
				//ST_SAMPLE_ENTR_NO = cmd1.GetString(1);
				ST_SAMPLE_NO = cmd1.GetString(1);
			}
			cmd1.Close();

			//异常信息
			cmd1.SetCommandText("SELECT ABNY_CODE,ABN_CONT  FROM TQMTS21  WHERE PONO = @pono ");
			cmd1.Parameters.Set("pono", tpssm10["PONO"]);
			cmd1.ExecuteReader();
			if (cmd1.Read())
			{
				ABNY_CODE = cmd1.GetString(1);
				ABN_CONT = cmd1.GetString(2);
			}
			cmd1.Close();

			//纯脱气时间
			cmd1.SetCommandText("SELECT PURE_DEGAS_DURATION  FROM TMMSM23  WHERE PONO = @pono "
				" AND PURE_DEGAS_DURATION > 0 ORDER  BY START_TIME DESC FETCH  FIRST 1 ROWS ONLY");
			cmd1.Parameters.Set("pono", tpssm10["PONO"]);
			cmd1.ExecuteReader();
			if (cmd1.Read())
			{
				PURE_DEGAS_DURATION = cmd1.GetDecimal(1);
			}
			cmd1.Close();


			//升温氧气耗量
			cmd1.SetCommandText("SELECT SUM(O2_SUM_COMSUME)  FROM TMMSM23  WHERE PONO = @pono ");
			cmd1.Parameters.Set("pono", tpssm10["PONO"]);
			cmd1.ExecuteReader();
			if (cmd1.Read())
			{
				O2_SUM_COMSUME = cmd1.GetDecimal(1);
			}
			cmd1.Close();

			//工艺卡信息
			cmd1.SetCommandText("SELECT *  FROM TQMTS0X  WHERE ST_NO = @st_no ");
			cmd1.Parameters.Set("st_no", tpssm10["ST_NO"]);
			cmd1.ExecuteReader();
			if (cmd1.Read())
			{
				cmd1.Fetch(tqmts0x);
			}
			cmd1.Close();
			

			//bcls_ret->Tables[0].Rows[i]["ST_SAMPLE_ENTR_NO"] = ST_SAMPLE_ENTR_NO;
			bcls_ret->Tables[0].Rows[i]["ST_SAMPLE_NO"] = ST_SAMPLE_NO;
			bcls_ret->Tables[0].Rows[i]["ABNY_CODE"] = ABNY_CODE;
			bcls_ret->Tables[0].Rows[i]["ABN_CONT"] = ABN_CONT;
			bcls_ret->Tables[0].Rows[i]["PURE_DEGAS_DURATION"] = PURE_DEGAS_DURATION;
			bcls_ret->Tables[0].Rows[i]["O2_SUM_COMSUME"] = O2_SUM_COMSUME;

			bcls_ret->Tables[0].Rows[i]["REFINE_ROUTE_CODE1"] = tqmts0x["REFINE_ROUTE_CODE"].ToString();
			bcls_ret->Tables[0].Rows[i]["SLAB_CHECK_CODE"] = tqmts0x["SLAB_CHECK_CODE"].ToString();
			//bcls_ret->Tables[0].Rows[i]["slab_sampling_code_sul"] = tqmts0x["slab_sampling_code_sul"].ToString();
			//bcls_ret->Tables[0].Rows[i]["slab_sampling_code_bil"] = tqmts0x["slab_sampling_code_bil"].ToString();
			//bcls_ret->Tables[0].Rows[i]["slab_dhcr_code"] = tqmts0x["slab_dhcr_code"].ToString();
			bcls_ret->Tables[0].Rows[i]["SLAB_FINISH_CODE"] = tqmts0x["SLAB_FINISH_CODE"].ToString();
			//bcls_ret->Tables[0].Rows[i]["slab_hdscarf_mode"] = tqmts0x["slab_hdscarf_mode"].ToString();
			//bcls_ret->Tables[0].Rows[i]["deal_gas_time_deci"] = tqmts0x["deal_gas_time_deci"].ToString();
			//bcls_ret->Tables[0].Rows[i]["ccc_num_code"] = tqmts0x["ccc_num_code"].ToString();
			//bcls_ret->Tables[0].Rows[i]["chemi_machine_code"] = tqmts0x["chemi_machine_code"].ToString();
			bcls_ret->Tables[0].Rows[i]["O_GAS_DIV"] = tqmts0x["O_GAS_DIV"].ToString();
			bcls_ret->Tables[0].Rows[i]["N_GAS_DIV"] = tqmts0x["N_GAS_DIV"].ToString();
			bcls_ret->Tables[0].Rows[i]["H_GAS_DIV"] = tqmts0x["H_GAS_DIV"].ToString();
			
		}

		//返回分页总数量信息

		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = rc;
		bcls_ret->Tables[0].set_TableName("QMTS21_QMTS21");
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


