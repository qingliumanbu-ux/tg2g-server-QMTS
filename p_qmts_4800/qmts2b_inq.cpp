/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-26
Description: 板坯钻样实绩查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中






// service入口
BM2F_ENTERACE(qmts2b_inq)


int f_qmts2b_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	
	int q_flag = 0;
	CString q_mat_no = " ";
	CString q_heat_no = " ";
	CString q_sample_pos_code = " ";
	CString q_rec_create_time_from = " ";
	CString q_rec_create_time_to = " ";
	CString q_analyse_time_from = " ";
	CString q_analyse_time_to = " ";
	CString q_st_no = " ";

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts2a("TQMTS2A");
	CModel tqmts2b("TQMTS2B");
	CModel tqmts2c("TQMTS2C");
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_tqmts2c(conn);

	try
	{
		//获得输入参数
		q_flag = bcls_rec->Tables[0].Rows[0]["flag"].ToDecimal().ToInt16();
		Log::Trace("", "","qmts2b_inq IN:---q_flag = [{0}]",q_flag);

		if(q_flag == 1)
		{
			q_mat_no = bcls_rec->Tables[0].Rows[0]["mat_no"].ToString().Trim();
			q_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
			q_sample_pos_code = bcls_rec->Tables[0].Rows[0]["sample_pos_code"].ToString().Trim();
			q_rec_create_time_from = bcls_rec->Tables[0].Rows[0]["rec_create_time_from"].ToString().Trim();
			q_rec_create_time_to = bcls_rec->Tables[0].Rows[0]["rec_create_time_to"].ToString().Trim();
			q_analyse_time_from = bcls_rec->Tables[0].Rows[0]["analyse_time_from"].ToString().Trim();
			q_analyse_time_to = bcls_rec->Tables[0].Rows[0]["analyse_time_to"].ToString().Trim();

			Log::Trace("", "","qmts2b_inq IN:---q_mat_no = [{0}]",(const char*)q_mat_no);
			Log::Trace("", "","qmts2b_inq IN:---q_heat_no = [{0}]",(const char*)q_heat_no);
			Log::Trace("", "","qmts2b_inq IN:---q_sample_pos_code = [{0}]",(const char*)q_sample_pos_code);
			Log::Trace("", "","qmts2b_inq IN:---q_rec_create_time_from[{0}], q_rec_create_time_to[{1}]", (const char*)q_rec_create_time_from, (const char*)q_rec_create_time_to);
			Log::Trace("", "","qmts2b_inq IN:---q_analyse_time_from[{0}], q_analyse_time_to[{1}]", (const char*)q_analyse_time_from, (const char*)q_analyse_time_to);

			bcls_ret->Tables.Add("TQMTS2A");
			bcls_ret->Tables["TQMTS2A"].Columns.Add(tqmts2a);

			Log::Trace("", "","SELECT TQMTS2A");
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
							 "   FROM TQMTS2A "
							 "  WHERE SLAB_NO LIKE @q_mat_no||'%' "
							 "    AND HEAT_NO LIKE @q_heat_no||'%' "
							 "    AND SAMPLE_POS_CODE LIKE @q_sample_pos_code||'%'  ";
				if ( q_rec_create_time_from.Trim() != "")  sqlstr = sqlstr + " AND substr(REC_CREATE_TIME,1,8) >= @q_rec_create_time_from||'000000' ";
				if ( q_rec_create_time_to.Trim() != "")  sqlstr = sqlstr + " AND substr(REC_CREATE_TIME,1,8) <= @q_rec_create_time_to||'999999' ";
				sqlstr = sqlstr + "    AND SEND_NUM > 0 "
							 "  ORDER BY SLAB_NO,SAMPLE_POS_CODE ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("q_mat_no", q_mat_no.Trim());
			cmd_inq.Parameters.Set("q_heat_no", q_heat_no.Trim());
			cmd_inq.Parameters.Set("q_sample_pos_code", q_sample_pos_code.Trim());
			cmd_inq.Parameters.Set("q_rec_create_time_from", q_rec_create_time_from.Trim());
			cmd_inq.Parameters.Set("q_rec_create_time_to", q_rec_create_time_to.Trim());
			//cmd_inq.Parameters.Set("q_rec_create_time_to", q_rec_create_time_to.Trim().Substring(0,8));
			cmd_inq.ExecuteQuery(bcls_ret->Tables["TQMTS2A"]);
			cmd_inq.Close();

			bcls_ret->Tables.Add("TQMTS2B");
			bcls_ret->Tables["TQMTS2B"].Columns.Add(tqmts2b);
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
							 "   FROM TEP0002 "
							 "  WHERE code_class = 'QMYS' "
							 "    AND  code_desc_2_content  is not null "
							 "  ORDER BY code_desc_2_content ASC ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tep0002); 

				bcls_ret->Tables["TQMTS2B"].Columns.Add(DT_DECIMAL,tep0002["CODE"].ToString());
			}
			cmd_inq.Close();

			Log::Trace("", "","SELECT TQMTS2B");
			Log::Trace("", "","q_mat_no = [{0}]",(const char*)q_mat_no);
			Log::Trace("", "","q_heat_no = [{0}]",(const char*)q_heat_no);
			Log::Trace("", "","q_sample_pos_code = [{0}]",(const char*)q_sample_pos_code);
			Log::Trace("", "","q_analyse_time_from[{0}], q_analyse_time_to[{1}]", (const char*)q_analyse_time_from, (const char*)q_analyse_time_to);
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
							 "   FROM TQMTS2B "
							 "  WHERE SLAB_NO LIKE @q_mat_no||'%' "
							 "    AND HEAT_NO LIKE @q_heat_no||'%' ";
					if ( q_sample_pos_code.Trim() != "")  sqlstr = sqlstr + " AND SAMPLE_POS_CODE = @q_sample_pos_code ";
					if ( q_analyse_time_from.Trim() != "")  sqlstr = sqlstr + " AND ANALYSE_TIME >= @q_analyse_time_from ";
					if ( q_analyse_time_to.Trim() != "")  sqlstr = sqlstr + "  AND ANALYSE_TIME <= @q_analyse_time_to ";
					sqlstr = sqlstr + "  ORDER BY SLAB_NO,SAMPLE_POS_CODE ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("q_mat_no", q_mat_no.Trim());
			cmd_inq.Parameters.Set("q_heat_no", q_heat_no.Trim());
			cmd_inq.Parameters.Set("q_sample_pos_code", q_sample_pos_code.Trim());
			cmd_inq.Parameters.Set("q_analyse_time_from", q_analyse_time_from.Trim());
			cmd_inq.Parameters.Set("q_analyse_time_to", q_analyse_time_to.Trim());
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts2b);
				Log::Trace("", "","ST_SAMPLE_NO = [{0}]",(const char*)tqmts2b["ST_SAMPLE_NO"].ToString());
				CDataRow& row = bcls_ret->Tables["TQMTS2B"].Rows.Add();
				row.Merge(tqmts2b);

				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT * "
								 "   FROM TQMTS2C "
								 "  WHERE ST_SAMPLE_NO = @tqmts2b.ST_SAMPLE_NO "
								 "  ORDER BY ELM_POS ";
						break;
				}
				cmd_tqmts2c.SetCommandText(sqlstr);
				cmd_tqmts2c.Parameters.Set("tqmts2b.ST_SAMPLE_NO", tqmts2b["ST_SAMPLE_NO"].ToString().Trim());
				cmd_tqmts2c.ExecuteReader();
				while (cmd_tqmts2c.Read())
				{
					cmd_tqmts2c.Fetch(tqmts2c);
					Log::Trace("", "","ELM_CODE[{0}],ELM_VALUE[{1}]",(const char*)tqmts2c["ELM_CODE"].ToString(),tqmts2c["ELM_VALUE"].ToDecimal().ToDouble());
					row[tqmts2c["ELM_CODE"].ToString()] = tqmts2c["ELM_VALUE"];
				}
				cmd_tqmts2c.Close();
			}
			cmd_inq.Close();
		}
		else
		{
			q_st_no = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
			Log::Trace("", "","qmts2b_inq IN:---q_st_no = [{0}]",(const char*)q_st_no);

			bcls_ret->Tables.Add("TQMTS02");
			bcls_ret->Tables["TQMTS02"].Columns.Add(tqmts02);

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT * "
							 "   FROM TQMTS02 "
							 "  WHERE ST_NO = @q_st_no "
							 "    AND WHOLE_BACKLOG_CODE = (select distinct ic_cc_flag from tqmts0x where st_no = @q_st_no) "
							 "  ORDER BY ELM_POS ASC ";
					break;
					//改动：从tqmts0x里用distinct仅取一个ic_cc_flag
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("q_st_no", q_st_no.Trim());
			cmd_inq.ExecuteQuery(bcls_ret->Tables["TQMTS02"]);
			cmd_inq.Close();
		}
		sprintf(s.msg,_RES("QM00S0004386")/*查询完毕。*/);
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
