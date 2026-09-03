/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      guxia
Version:     1.0
Date:        2015-07-01
Description: 修改炉次最终出钢记号
20230530  在产品化基础上按照梅钢规则进行判定
**************************************************/


//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件，请包含在""中

/*  外部函数申明  */
int f_qmts_jud_pono(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

// service入口
BM2F_ENTERACE(qmts21m_jud)


int f_qmts21m_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	int mat_count = 0;
	CDecimal mat_act_wt = 0;

	CString s_heat_no = " ";

	CString sqlstr = "";
	CString sqlstr1 = "";

	

	//调用炉次判定函数用
	EIClass bcls_rec_f1;
	EIClass bcls_ret_f1;
	bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "MAIN_BACKLOG_CODE");
	bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "PREC_ST_NO");
	bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "DECI_ST_NO");
	bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "FIN_ST_NO");
	bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "YY000000_CAUSE");
	bcls_rec_f1.Tables[0].Columns.Add(DT_STRING, "RE_CHECK_FLAG");
	bcls_rec_f1.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts23("TQMTS23");
	CModel tqmts29("TQMTS29");
	CModel tmmsm01("TMMSM01");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq1(conn);
	CDbCommand cmd_upd(conn);
	try
	{
		//获得输入参数
		s_heat_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		CString jud_flag = bcls_rec->Tables[0].Rows[0]["jud_flag"].ToString().Trim();
		CString main_backlog_code = bcls_rec->Tables[0].Rows[0]["main_backlog_code"].ToString().Trim();
		CString pono = bcls_rec->Tables[0].Rows[0]["pono"].ToString().Trim();
		CString pono_st_sample_entr_no = bcls_rec->Tables[0].Rows[0]["heat_no"].ToString().Trim();
		CString pono_st_sample_no = bcls_rec->Tables[0].Rows[0]["pono_st_sample_no"].ToString().Trim();
		CString pono_prec_st_no = bcls_rec->Tables[0].Rows[0]["pono_prec_st_no"].ToString().Trim();
		CString pono_deci_st_no = bcls_rec->Tables[0].Rows[0]["pono_deci_st_no"].ToString().Trim();
		CString pono_fin_st_no = bcls_rec->Tables[0].Rows[0]["pono_fin_st_no"].ToString().Trim();
		CString pono_yy000000_cause = bcls_rec->Tables[0].Rows[0]["pono_yy000000_cause"].ToString().Trim();
		CString pono_re_check_flag = bcls_rec->Tables[0].Rows[0]["pono_re_check_flag"].ToString().Trim();
		/*TQMTS21表不在记录这些数据都空着吧
		CString pono_abny_code = bcls_rec->Tables[0].Rows[0]["pono_abny_code"].ToString().Trim();
		CString pono_abn_cont = bcls_rec->Tables[0].Rows[0]["pono_abn_cont"].ToString().Trim();*/

		Log::Trace("", "", "qmts21_jud IN:---jud_flag = [{0}]", jud_flag);
		Log::Trace("", "", "qmts21_jud IN:---s_heat_no = [{0}]", s_heat_no);
		Log::Trace("", "", "qmts21_jud IN:---pono_fin_st_no = [{0}]", pono_fin_st_no);

		//-----------合理性检查------------------
		if (s_heat_no.Trim() == "")
		{
			strcpy(s.msg, "熔炼号不允许为空");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//if (s_fin_st_no.Trim() == "")
		//{
		//	sprintf(s.msg, _RES("QM00S0004171")/*出钢记号不可为空。*/);
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}
		if (jud_flag.Trim()== "")
		{
			sprintf(s.msg, "冷/热检标记不可为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (pono.Trim() == "")
		{
			sprintf(s.msg, "制造命令号不可为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (strcmp(pono_prec_st_no, " ") == 0)
		{
			sprintf(s.msg, "预定出钢记号不可为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (pono_deci_st_no.Trim() == "")
		{
			sprintf(s.msg, "决定出钢记号不可为空!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (jud_flag == "0")//热检
		{
			CString HEAT_CONFM_FLAG = " ";
			CString PREC_ST_NO = " ";
			CString DECI_ST_NO = " ";
			CString FIN_ST_NO = " ";
			CString SM_PLAN_NO = " ";
			CString HEAT_NO = " ";
			CDecimal ponoCount = 0;
			//计划信息
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT HEAT_CONFM_TIME, HEAT_NO "
					" FROM  TPSSM11 "
					"   WHERE  PONO = @pono ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("pono", pono.Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				HEAT_CONFM_FLAG = cmd_inq.GetString(1);
				HEAT_NO = cmd_inq.GetString(2);
			}
			cmd_inq.Close();
			if (HEAT_NO.Trim()=="")
			{
				sprintf(s.msg, "制造命令号[%s]不存在或已炉次确定，请刷新画面!", (const char*)pono);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//炉次钢种信息
			Log::Trace("", "", "炉次钢种信息");
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT  ST_NO, DECI_ST_NO, FIN_ST_NO, SM_PLAN_NO "
					" FROM  TQMTS23 "
					"   WHERE  HEAT_NO = @s_heat_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("s_heat_no", s_heat_no.Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				
				PREC_ST_NO = cmd_inq.GetString(1);
				DECI_ST_NO = cmd_inq.GetString(2);
				FIN_ST_NO = cmd_inq.GetString(3);
				SM_PLAN_NO = cmd_inq.GetString(4);
			}
			cmd_inq.Close();
			Log::Trace("", "", "TQMTS23表中PREC_ST_NO = {0}, DECI_ST_NO = {1}", PREC_ST_NO, DECI_ST_NO);
			if (pono_deci_st_no== PREC_ST_NO || pono_deci_st_no=="YY000000")
			{
				if (pono_deci_st_no.Substring(0, 2) == "YY" )
				{
					if (pono_st_sample_no.Trim()== "")
					{
						//zxl
					}
					else
					{
						sprintf(s.msg, "决定出钢记号是YY待判,代表样为空!");
						throw CApplicationException(-1, s.msg, log.Location);
					}

					if (pono_yy000000_cause== "")
					{
						sprintf(s.msg, "YY判定原因未选定!");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				else
				{
					if (pono_st_sample_entr_no== "" || pono_st_sample_no== "")
					{
						sprintf(s.msg, "决定出钢记号不是YY待判,选择代表样!");
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else
			{
				sprintf(s.msg, "决定出钢记号不是预定出钢记号或YY待判!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//只要炉次下的板坯有送出，炉次热检就不可在修改了
			
			CDecimal tmmsm01_num = 0;
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT  COUNT(MAT_NO) "
					" FROM  TMMSM01 "
					"   WHERE  PONO = @pono "
					"   AND  MAT_POSITION NOT IN('1', '2')";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("pono", pono.Trim());
			tmmsm01_num = cmd_inq.ExecuteScalar();
			cmd_inq.Close();
			Log::Trace("", "", "已经送热轧的板坯{0}", tmmsm01_num);
			if (tmmsm01_num > 0)
			{
				sprintf(s.msg, "制造命令号[%s]已有板坯送出,不可再次热检判定!", (const char*)pono);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else if (jud_flag == '1')//冷检
		{
			CString HEAT_CONFM_TIME = " ";
			CString PREC_ST_NO = " ";
			CString DECI_ST_NO = " ";
			CString FIN_ST_NO = " ";
			CString SM_PLAN_NO = " ";
			CString HEAT_NO = " ";
			//冷热检区分
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT HEAT_CONFM_TIME, HEAT_NO "
					" FROM  TPSSM41 "
					"   WHERE HEAT_NO = @s_heat_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("s_heat_no", s_heat_no.Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				HEAT_CONFM_TIME = cmd_inq.GetString(1);
				HEAT_NO = cmd_inq.GetString(2);
			}
			cmd_inq.Close();
			if (HEAT_NO.Trim() == "")
			{
				sprintf(s.msg, "制造命令号[%s]在炼钢计划中不存在，请刷新画面!", (const char*)pono);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//炉次钢种信息
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT  ST_NO, DECI_ST_NO, FIN_ST_NO, SM_PLAN_NO "
					" FROM  TQMTS23 "
					"   WHERE  HEAT_NO = @s_heat_no ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("s_heat_no", s_heat_no.Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{

				PREC_ST_NO = cmd_inq.GetString(1);
				DECI_ST_NO = cmd_inq.GetString(2);
				FIN_ST_NO = cmd_inq.GetString(3);
				SM_PLAN_NO = cmd_inq.GetString(4);
			}
			cmd_inq.Close();

			if (pono_fin_st_no == "")
			{
				sprintf(s.msg, "最终出钢记号不可为空!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (pono_fin_st_no.Substring(0, 2) == "YY")
			{
				sprintf(s.msg, "最终出钢记号不可为YY待判!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (HEAT_CONFM_TIME.Trim() == "")
			{
				sprintf(s.msg, "制造命令号[%s]尚未炉次确定，不能最终出钢记号判定!", (const char*)pono);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (DECI_ST_NO.Trim()!= "YY000000")
			{
				sprintf(s.msg, "制造命令号[%s]炉次确定时，决定出钢记号不为YY待判，无需最终出钢记号判定!", (const char*)pono);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (FIN_ST_NO.Trim() != " ")
			{
				sprintf(s.msg, "制造命令号[%s]已经做过最终出钢记号判定!", (const char*)pono);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//炉次冷检时需选代表样
			if (pono_st_sample_no.Trim() == "")
			{
				sprintf(s.msg, "代表样不可为空!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//校验最终出钢记号是否存在
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
			cmd_inq.Parameters.Set("st_no", pono_fin_st_no.Trim());
			i_tqmts0x_num = cmd_inq.ExecuteScalar();
			cmd_inq.Close();
			if (i_tqmts0x_num <= 0)
			{
				CFormattable arguments[] = { pono_fin_st_no }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, _RES("QM00S0004000")/*出钢记号[{0}]在工艺卡中不存在，不能操作。*/, arguments, 1); //格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{
			sprintf(s.msg, "冷/热检标记不正确!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", "", "3-炉次判定");
		//3-炉次判定
		bcls_rec_f1.Tables[0].Rows[0]["MAIN_BACKLOG_CODE"] = main_backlog_code;
		bcls_rec_f1.Tables[0].Rows[0]["PONO"] = pono;
		bcls_rec_f1.Tables[0].Rows[0]["HEAT_NO"] = pono_st_sample_entr_no;
		bcls_rec_f1.Tables[0].Rows[0]["ST_SAMPLE_NO"] = pono_st_sample_no;
		bcls_rec_f1.Tables[0].Rows[0]["PREC_ST_NO"] = pono_prec_st_no;
		bcls_rec_f1.Tables[0].Rows[0]["DECI_ST_NO"] = pono_deci_st_no;
		bcls_rec_f1.Tables[0].Rows[0]["FIN_ST_NO"] = pono_fin_st_no;
		bcls_rec_f1.Tables[0].Rows[0]["YY000000_CAUSE"] = pono_yy000000_cause;
		bcls_rec_f1.Tables[0].Rows[0]["RE_CHECK_FLAG"] = pono_re_check_flag;

		//调用炉次判定函数函数
		bcls_rec_f1.SetSYS(s);
		doFlag = f_qmts_jud_pono(&bcls_rec_f1, &bcls_ret_f1,conn);
		if (doFlag != 0)
		{
			bcls_ret_f1.GetSYS(&s);
			EDLog(1, 1, "f_qmts_jud_pono() msg = [%s]", s.msg);
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
