/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-18
Description: 炼钢成分收集及判定
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中
/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炼钢成分收集及判定
/// <para>
/// 1.炼钢成分收集及判定。
/// </para>
/// <para>主调用函数：
/// <para>需调用函数：
/// </summary>
/// <param name="heat_no">熔炼号						</param>
/// <param name="st_sample_no">炼钢试样号               </param>
//获取/检查传入参数, 合理性检查:熔炼号、炼钢试样号不允许为空……
//写炉次信息表TQMTS23（f_qmts23_ins）
//写成分主子表：TQMTS24  TQMTS25 (补填来自QMYS小代码的信息，合并气体样)  如果是渣样，写的是TQMTS26
//判定成分是否合格 f_qmts_jud（此判定函数中包含组合元素的计算）
//如果是连铸工序的合格样，则自动选为代表样
/// <returns>  </returns>
===========================================================</remark>*/


/* ***** 外部函数申明 ***** */

BM2_FUNCTION_IMPORT
int f_qmts_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//成分判定
BM2_FUNCTION_IMPORT
int f_qmts23_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);
BM2_FUNCTION_IMPORT
int f_qmts_rep_upd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); //代表成分选择函数
BM2_FUNCTION_EXPORT
int f_qmts_cf_rev(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	int elm_num = 0;//实绩元素数量

	CString sqlstr = "";
	CString s_factory_div = "";
	//调用函数用Block
	EIClass bcls_rec_f;
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f.Tables[0].Rows.Add();

	//调用函数用Block
	EIClass bcls_rec_f23;
	EIClass bcls_ret_f23;
	bcls_rec_f23.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f23.Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NO");
	bcls_rec_f23.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_f23.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f23.Tables[0].Rows.Add();

	//调代表成分
	EIClass bcls_rec_rep;
	EIClass bcls_ret_rep;
	bcls_rec_rep.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_rep.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_rep.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
	bcls_rec_rep.Tables[0].Rows.Add();
	/* 实体类定义 */
	CModel tqmts23("TQMTS23");
	CModel tqmts24("TQMTS24");
	CModel tqmts25("TQMTS25");
	CModel tqmts26("TQMTS26");
	CModel tqmts0x("TQMTS0X");
	CModel tpssm11("TPSSM11");
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 获得输入参数 */
		tqmts24.Reset();
		tqmts24.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tqmts25.Reset();
		tqmts25["ELM_BACKUP"] = (int)bcls_rec->Tables[0].Rows[0]["ELM_BACKUP"];

		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---heat_no[{0}]", (const char*)tqmts24["HEAT_NO"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---st_sample_no[{0}]", (const char*)tqmts24["ST_SAMPLE_NO"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---whole_backlog_code[{0}]", (const char*)tqmts24["WHOLE_BACKLOG_CODE"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---st_sample_div[{0}]", (const char*)tqmts24["ST_SAMPLE_DIV"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---gas_type_div[{0}]", (const char*)tqmts24["GAS_TYPE_DIV"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---st_sample_seq[{0}]", tqmts24["ST_SAMPLE_SEQ"].ToDecimal().ToInt32());
		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---analyse_time[{0}]", (const char*)tqmts24["ANALYSE_TIME"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---sample_ifgood[{0}]", (const char*)tqmts24["SAMPLE_IFGOOD"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---c_s_judge_div[{0}]", (const char*)tqmts24["C_S_JUDGE_DIV"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---elm_backup[{0}]", tqmts25["ELM_BACKUP"].ToDecimal().ToInt32());

		//校验传入参数
		if (tqmts24["HEAT_NO"].ToString().TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[熔炼号不允许为空]");
			strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tqmts24["ST_SAMPLE_NO"].ToString().TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[炼钢试样号不允许为空]");
			strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tqmts24["ST_SAMPLE_DIV"].ToString().TrimOrBlank() == " ")
		{
			Log::Trace("", __FUNCTION__, "ERROR------[试样区分不允许为空]");
			strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tqmts24["ST_SAMPLE_DIV"].ToString() != "3")
		{
			if (tqmts24["WHOLE_BACKLOG_CODE"].ToString().TrimOrBlank() == " ")
			{
				Log::Trace("", __FUNCTION__, "ERROR------[工序不允许为空]");
				strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (tqmts24["ST_SAMPLE_DIV"].ToString().TrimOrBlank() == "4")
		{
			if (tqmts24["GAS_TYPE_DIV"].ToString().TrimOrBlank() == " ")
			{
				Log::Trace("", __FUNCTION__, "ERROR------[气体区分不允许为空]");
				strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (tqmts24["ST_SAMPLE_SEQ"].ToDecimal() == 0)
		{
			Log::Trace("", __FUNCTION__, "ERROR------[试样顺序号不允许为零]");
			strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		////////查询厂别，update by yiling 20170106
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT ST_NO,FACTORY_DIV FROM"
				" (SELECT ST_NO,FACTORY_DIV FROM TPSSM11 WHERE HEAT_NO = @heat_no "
				" UNION "
				"  SELECT ST_NO,FACTORY_DIV FROM TPSSM41 WHERE HEAT_NO = @heat_no )"
				;
			break;
		}
		Log::Trace("", "", "sqlstr = [{0}]", sqlstr);

		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			s_factory_div = cmd_inq.GetString(2).Trim().Substring(1, 1);
			Log::Trace("", __FUNCTION__, "s_factory_div		= [{0}]", s_factory_div);

			if (s_factory_div == "1")
			{
				s_factory_div = "A";
			}
			else
			{
				s_factory_div = "B";
			}
		}
		cmd_inq.Close();
		tqmts0x["FACTORY_DIV"] = s_factory_div;
		//tqmts0x["ST_NO"] = tqmts24["ST_NO"];
		//tqmts0x.Query("ST_NO,FACTORY_DIV");
		//==================================================================================================
		//成分主信息TQMTS24
		//赋初值
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT REC_CREATOR,REC_CREATE_TIME "
				"  FROM TQMTS24 "
				" WHERE HEAT_NO = @heat_no "
				"   AND ST_SAMPLE_NO = @st_sample_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("st_sample_no", tqmts24["ST_SAMPLE_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts24["REC_CREATOR"] = cmd_inq.GetString(1);
			tqmts24["REC_CREATE_TIME"] = cmd_inq.GetString(2);
			tqmts24["REC_REVISOR"] = s.userid;
			tqmts24["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts24.Delete("HEAT_NO,ST_SAMPLE_NO");
		}
		else
		{
			tqmts24["REC_CREATOR"] = s.userid;
			tqmts24["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts24["REC_REVISOR"] = " ";
			tqmts24["REC_REVISE_TIME"] = " ";
		}
		cmd_inq.Close();
		tqmts24["ARCHIVE_FLAG"] = " ";
		tqmts24["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];

		////读取炉次信息
		//switch(conn->DatabaseKind)
		//{
		//case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//case DB_KIND_MSSQL:				// MS SQL Server数据库
		//case DB_KIND_ORACLE:	        // Oracle 数据库
		//default:						// 所有数据库适用，通用SQL语句
		//	sqlstr = "SELECT PONO,SM_PLAN_NO,ST_NO,FIN_ST_NO "
		//		"  FROM TQMTS23 "
		//		" WHERE HEAT_NO = @heat_no ";
		//	break;
		//}
		//cmd_inq.SetCommandText(sqlstr);		
		//cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString());
		//cmd_inq.ExecuteReader();
		//if (cmd_inq.Read())
		//{
		//	tqmts24["PONO"] = cmd_inq.GetString(1);
		//	tqmts24["SM_PLAN_NO"] = cmd_inq.GetString(2);
		//	tqmts24["ST_NO"] = cmd_inq.GetString(3);
		//	tqmts24["FIN_ST_NO"] = cmd_inq.GetString(4);
		//}
		//else
		//{
		//	cmd_inq.Close();
		
		

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT ST_NO ,SM_PLAN_NO,PONO FROM "
					"(SELECT * FROM TPSSM11 WHERE 1=1  ";
				if (tqmts24["HEAT_NO"].ToString().Trim() != "")	sqlstr += " AND HEAT_NO like '%" + tqmts24["HEAT_NO"].ToString().Trim() + "%' ";
				if (tqmts24["PONO"].ToString().Trim() != "")		sqlstr += " AND PONO    like '%" + tqmts24["PONO"].ToString().Trim() + "%' ";
				sqlstr += "UNION SELECT * FROM TPSSM41  WHERE 1 = 1";
				if (tqmts24["HEAT_NO"].ToString().Trim() != "")	sqlstr += " AND HEAT_NO like '%" + tqmts24["HEAT_NO"].ToString().Trim() + "%' ";
				if (tqmts24["PONO"].ToString().Trim() != "")		sqlstr += " AND PONO    like '%" + tqmts24["PONO"].ToString().Trim() + "%' ";
				sqlstr += ") ORDER BY CAST_NO ASC, CAST_DIV_NO ASC ";
				break;
			}

			Log::Trace("", "", "sqlstr=[{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts23["HEAT_NO"] = tqmts24["HEAT_NO"];
				tqmts23["ST_NO"] = cmd_inq.GetString(1);
				tqmts23["SM_PLAN_NO"] = cmd_inq.GetString(2);
				tqmts23["PONO"] = cmd_inq.GetString(3);

				tqmts24["PONO"] = tqmts23["PONO"];
				tqmts24["ST_NO"] = tqmts23["ST_NO"];
				

				bcls_rec_f23.Tables[0].Rows[0]["HEAT_NO"] = tqmts23["HEAT_NO"];
				bcls_rec_f23.Tables[0].Rows[0]["ST_NO"] = tqmts23["ST_NO"];
				bcls_rec_f23.Tables[0].Rows[0]["PONO"] = tqmts23["PONO"];
				bcls_rec_f23.Tables[0].Rows[0]["SM_PLAN_NO"] = tqmts23["SM_PLAN_NO"];
				Log::Trace("", __FUNCTION__, " tqmts23.HEAT_NO[{0}] tqmts23.ST_NO[{1}]tqmts23.PONO[{2}]tqmts23.SM_PLAN_NO[{3}]", (const char*)tqmts23["HEAT_NO"].ToString(), (const char*)tqmts23["ST_NO"].ToString(), (const char*)tqmts23["PONO"].ToString(), (const char*)tqmts23["SM_PLAN_NO"].ToString());

				doFlag = f_qmts23_ins(&bcls_rec_f23, &bcls_ret_f23, conn);
				if (doFlag != 0)
				{
					Log::Trace("", __FUNCTION__, "f_qmts23_ins() msg = [{0}]", s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				
			}
			else
			{
				sprintf(s.msg, "熔炼号[{0}]在炉次质量信息表[TQMTS23]中不存在！", (const char*)tqmts24["HEAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();
		
		/*}
		cmd_inq.Close();*/

		//==================================================================================================
		//成分子信息TQMTS26(渣样)
		if (tqmts24["ST_SAMPLE_DIV"].ToString() == "3")
		{
			Log::Trace("", __FUNCTION__, "INSERT TQMTS26");
			//赋初值
			tqmts26["REC_CREATOR"] = tqmts24["REC_CREATOR"];
			tqmts26["REC_CREATE_TIME"] = tqmts24["REC_CREATE_TIME"];
			tqmts26["REC_REVISOR"] = tqmts24["REC_REVISOR"];
			tqmts26["REC_REVISE_TIME"] = tqmts24["REC_REVISE_TIME"];
			tqmts26["ARCHIVE_FLAG"] = tqmts24["ARCHIVE_FLAG"];
			tqmts26["PONO"] = tqmts24["PONO"];
			tqmts26["HEAT_NO"] = tqmts24["HEAT_NO"];
			tqmts26["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
			tqmts26["ST_SAMPLE_NAME"] = " ";
			tqmts26["ST_NO"] = tqmts24["ST_NO"];
			tqmts26["WHOLE_BACKLOG_CODE"] = tqmts24["WHOLE_BACKLOG_CODE"];

			sqlstr = "tqmts26.Delete(HEAT_NO, ST_SAMPLE_NO)";
			tqmts26.Delete("HEAT_NO, ST_SAMPLE_NO");

			//获取各元素值并插渣样子表
			for (i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
			{
				tqmts26["ELM_CODE"] = bcls_rec->Tables[1].Rows[i]["ELM_CODE"].ToString().TrimOrBlank();
				tqmts26["ELM_ACT"] = bcls_rec->Tables[1].Rows[i]["ELM_ACT"].ToDecimal();

				Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---tqmts26.ELM_CODE[{0}]", (const char*)tqmts26["ELM_CODE"].ToString());
	//			Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:-- -tqmts26["ELM_ACT"].ToDecimal()[{0}]", tqmts26["ELM_ACT"].ToDecimal().ToDouble());

				if (tqmts26["ELM_ACT"].ToDecimal() != -1)
				{
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT CODE_DESC_1_CONTENT "
							"  FROM TEP0002 "
							" WHERE CODE_CLASS = 'QMZS' "
							"   AND CODE = @elm_code";
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("elm_code", tqmts26["ELM_CODE"].ToString());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						tqmts26["ELM_NAME"] = cmd_inq.GetString(1);
					}
					cmd_inq.Close();

					elm_num++;
					tqmts26.TrimOrBlank();
					Log::Trace("", __FUNCTION__, "tqmts26.Insert()");
					tqmts26.Insert();
				}
			}

			if (elm_num<1)
			{
				strcpy(s.msg, _RES("QM00S0004336")/*元素实绩不能全为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		//==================================================================================================
		//成分子信息TQMTS25
		else
		{
			Log::Trace("", __FUNCTION__, "INSERT TQMTS25");

			//赋初值
			tqmts25["REC_CREATOR"] = tqmts24["REC_CREATOR"];
			tqmts25["REC_CREATE_TIME"] = tqmts24["REC_CREATE_TIME"];
			tqmts25["REC_REVISOR"] = tqmts24["REC_REVISOR"];
			tqmts25["REC_REVISE_TIME"] = tqmts24["REC_REVISE_TIME"];
			tqmts25["ARCHIVE_FLAG"] = tqmts24["ARCHIVE_FLAG"];
			tqmts25["PONO"] = tqmts24["PONO"];
			tqmts25["HEAT_NO"] = tqmts24["HEAT_NO"];
			tqmts25["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
			tqmts25["ST_SAMPLE_NAME"] = " ";
			tqmts25["ST_NO"] = tqmts24["ST_NO"];
			tqmts25["WHOLE_BACKLOG_CODE"] = tqmts24["WHOLE_BACKLOG_CODE"];
			tqmts25["ST_SAMPLE_TYPE"] = tqmts24["ST_SAMPLE_DIV"];
			tqmts25["ELM_OK"] = 0;

			sqlstr = "tqmts25.Delete(HEAT_NO, ST_SAMPLE_NO)";
			tqmts25.Delete("HEAT_NO, ST_SAMPLE_NO");

			//获取各元素值并插试样子表
			for (i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
			{
				tqmts25["ELM_CODE"] = bcls_rec->Tables[1].Rows[i]["ELM_CODE"].ToString().TrimOrBlank();
				tqmts25["ELM_ACT"] = bcls_rec->Tables[1].Rows[i]["ELM_ACT"].ToDecimal();
				tqmts25["ELM_ACT_OLD"] = tqmts25["ELM_ACT"];///update by yiling 20170802 原始值保留。
				Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---tqmts25.ELM_CODE[{0}]", (const char*)tqmts25["ELM_CODE"].ToString());
		//		Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:-- -tqmts25["ELM_ACT"].ToDecimal()[{0}]", tqmts25["ELM_ACT"].ToDecimal().ToDouble());

				if (tqmts25["ELM_ACT"].ToDecimal() != -1)
				{
					if (tqmts24["ST_SAMPLE_DIV"].ToString() == "1")  //钢样
					{
						//if (tqmts25["ELM_CODE"].ToString() == "016" || tqmts25["ELM_CODE"].ToString() == "014" || tqmts25["ELM_CODE"].ToString() == "001")
						//{
						//	continue;
						//}
					}
					//else if (tqmts24["ST_SAMPLE_DIV"].ToString() == "4" && tqmts24["GAS_TYPE_DIV"].ToString() == "1")  //1-气体ON样;
					//{
					//	if (tqmts25["ELM_CODE"].ToString() != "016" && tqmts25["ELM_CODE"].ToString() != "014")
					//	{
					//		continue;
					//	}
					//}
					//else if (tqmts24["ST_SAMPLE_DIV"].ToString() == "4" && tqmts24["GAS_TYPE_DIV"].ToString() == "2" && tqmts25["ELM_CODE"].ToString() != "001")  //2-气体H样
					//{
					//	continue;
					//}

					else if (tqmts24["ST_SAMPLE_DIV"].ToString() == "4")  //1-气体ONH样合并一起;
					{
						if (tqmts25["ELM_CODE"].ToString() != "016" && tqmts25["ELM_CODE"].ToString() != "014" && tqmts25["ELM_CODE"].ToString() != "001")
						{
							continue;
						}
					}

					Log::Trace("", __FUNCTION__, "f_qmts_cf_rev IN:---tqmts25.ELM_CODE[{0}]---INSERT", (const char*)tqmts25["ELM_CODE"].ToString());
					switch (conn->DatabaseKind)
					{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT CODE_DESC_1_CONTENT "
							"  FROM TEP0002 "
							" WHERE CODE_CLASS = 'QMYS' "
							"   AND CODE = @elm_code"
							"  ";
						break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("elm_code", tqmts25["ELM_CODE"].ToString());
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						tqmts25["ELM_NAME"] = cmd_inq.GetString(1);
					}
					cmd_inq.Close();

					elm_num++;
					tqmts25.TrimOrBlank();

					Log::Trace("", __FUNCTION__, "tqmts25.Insert()");
					tqmts25.Insert();

				}
			}
			if (elm_num<1)
			{
				strcpy(s.msg, _RES("QM00S0004336")/*元素实绩不能全为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		//插入试样主记录
		tqmts24.TrimOrBlank();
		Log::Trace("", __FUNCTION__, "tqmts24.Insert()");
		Log::Trace("", "", "11111111tqmts24.st_sample_div", tqmts24["ST_SAMPLE_DIV"]);
		tqmts24.Insert();
		//判定==================================================================================================
		if (tqmts24["ST_SAMPLE_DIV"].ToString() != "3")
		{
			bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS25";
			bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = tqmts24["HEAT_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = tqmts24["ST_NO"];
			bcls_rec_f.Tables[0].Rows[0]["PONO"] = tqmts24["PONO"];

			//计算组合元素
			Log::Trace("", __FUNCTION__, "计算组合元素");
			Log::Trace("", __FUNCTION__, "tqmts24.HEAT_NO[{0}]", tqmts24["HEAT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "tqmts24.ST_SAMPLE_NO[{0}]", tqmts24["ST_SAMPLE_NO"].ToString());
			Log::Trace("", __FUNCTION__, "tqmts24.ST_NO[{0}]", tqmts24["ST_NO"].ToString());

			//试样良的话参与成分判定
			Log::Trace("", __FUNCTION__, "tqmts24.SAMPLE_IFGOOD[{0}]", tqmts24["SAMPLE_IFGOOD"].ToString());
			Log::Trace("", __FUNCTION__, "tqmts24.ST_SAMPLE_DIV[{0}]", tqmts24["ST_SAMPLE_DIV"].ToString());
			if (tqmts24["SAMPLE_IFGOOD"].ToString() == "1" && (tqmts24["ST_SAMPLE_DIV"].ToString() == "1" || tqmts24["ST_SAMPLE_DIV"].ToString() == "4" || tqmts24["ST_SAMPLE_DIV"].ToString() == "5"))
			{
				doFlag = f_qmts_jud(&bcls_rec_f, &bcls_ret_f, conn);
				if (doFlag != 0)
				{
					Log::Trace("", __FUNCTION__, "f_qmts_jud() msg = [{0}]", s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (tqmts24["WHOLE_BACKLOG_CODE"].ToString().TrimOrBlank() == "C")//自动选择连铸合格样为代表样
			{

				tqmts24["PONO"] = tqmts25["PONO"].ToString();
				tqmts24["REP_ELM_SEL_FLAG"] = "1";
				int i_count_tqmts24 = tqmts24.QueryCount("PONO,REP_ELM_SEL_FLAG");
				Log::Trace("", __FUNCTION__, "i_count_tqmts24[{0}]", i_count_tqmts24);
				if (i_count_tqmts24 == 0)
				{
					tqmts24["WHOLE_BACKLOG_CODE"] = "C";
					tqmts24.Query("PONO,HEAT_NO,WHOLE_BACKLOG_CODE,ST_SAMPLE_NO");
					Log::Trace("", __FUNCTION__, "tqmts24.JUDGE_CODE[{0}]PONO[{1}]ST_SAMPLE_NO[{2}]", tqmts24["JUDGE_CODE"].ToString(), tqmts24["PONO"].ToString(), tqmts25["ST_SAMPLE_NO"].ToString());
					if (tqmts24["JUDGE_CODE"].ToString().Trim() == "1")
					{
						//修改代表标记
						tqmts24["REP_ELM_SEL_FLAG"] = "1";
						tqmts24["ST_SAMPLE_DIV"] = "1";//代表样不管是钢样还是气体样最后都修改为1钢样
						tqmts24.Update("REP_ELM_SEL_FLAG,ST_SAMPLE_DIV",  //修改字段项
							"FACTORY_DIV, PONO, ST_SAMPLE_NO"); //条件字段项
						Log::Trace("", "", "tqmts24.FACTORY_DIV[{0}]",  tqmts24["FACTORY_DIV"].ToString());

						//调用代表成分选定函数
						bcls_rec_rep.Tables[0].Rows[0]["PONO"] = tqmts25["PONO"].ToString();
						bcls_rec_rep.Tables[0].Rows[0]["HEAT_NO"] = tqmts25["HEAT_NO"].ToString();
						bcls_rec_rep.Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts25["ST_SAMPLE_NO"].ToString();

						doFlag = f_qmts_rep_upd(&bcls_rec_rep, &bcls_ret_rep, conn);
						if (doFlag != 0)
						{
							Log::Trace("", "", "f_qmts_rep_upd() msg = [{0}]", s.msg);
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
				}
			}
		}//if ST_SAMPLE_DIV != "3"
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
