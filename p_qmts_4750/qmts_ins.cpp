/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 新增制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中













/*<remark>=========================================================
/// <summary>
/// 新增制造标准
/// <para>
/// 获取输入参数：table_name(制造标准表名)/st_no(出钢记号)/whole_backlog_code(炼钢工序)/flag(新增标记——0:制造标准;1:成分标准)
/// </para>
/// <para>数据库表：TQMTS0x(各个制造标准表) 
/// 				TQMTS02(工序成分标准表)
/// 				TQMTS01(工序制造标准表)
/// 前台各个QMTS0x画面的F3(新增)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各个制造标准表    </param>
===========================================================</remark>*/ 

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmts_ins)


int f_qmts_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int flag = 0;

	CString table_name = " ";
	CString q_st_no = " ";
	CString q_whole_backlog_code = " ";
	CString base_code = "";
	CDecimal v_count = 0;
	CString s_equ_no = " ";
	CString s_ingot_code = " ";
	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");
	CModel tqmts03("TQMTS03");
	CModel tqmts04("TQMTS04");
	CModel tqmts05("TQMTS05");
	CModel tqmts05p("TQMTS05P");
	CModel tqmts06("TQMTS06");
	CModel tqmts07("TQMTS07");
	CModel tqmts08("TQMTS08");
	CModel tqmts0a("TQMTS0A");
	CModel tqmts0m("TQMTS0M");
	CModel tqmts0l("TQMTS0L");
	CModel tqmts0x("TQMTS0X");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		table_name = bcls_rec->Tables[1].Rows[0]["table_name"].ToString().ToLower().Trim();
		q_st_no = bcls_rec->Tables[1].Rows[0]["st_no"].ToString().Trim();
		base_code = bcls_rec->Tables[1].Rows[0]["base_code"].ToString().Trim();
		q_whole_backlog_code = bcls_rec->Tables[1].Rows[0]["whole_backlog_code"].ToString().Trim();
		//q_whole_backlog_code = bcls_rec->Tables[1].Rows[0]["whole_backlog_code"].ToString().Trim();
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[1].Rows[0]["factory_div"].ToString().TrimOrBlank();  ////update by yiling 20170224
		//flag = bcls_rec->Tables[1].Rows[0]["flag"].ToDecimal().ToInt16();
		if (bcls_rec->Tables[1].Columns.Contains("equ_no"))
		{
			s_equ_no = bcls_rec->Tables[1].Rows[0]["equ_no"].ToString().TrimOrBlank();
			Log::Trace("", "", "s_equ_no[{0}]", s_equ_no);
		}
		if (bcls_rec->Tables[1].Columns.Contains("ingot_code"))
		{
			s_ingot_code = bcls_rec->Tables[1].Rows[0]["ingot_code"].ToString().TrimOrBlank();
			Log::Trace("", "", "s_ingot_code[{0}]", s_ingot_code);
		}
		Log::Trace("", "", "qmts_ins IN: table_name[{0}]st_no[{1}]whole_backlog_code[{2}]flag[{3}]q_factory_div[{4}]base_code[{5}]", (const char*)table_name, (const char*)q_st_no, (const char*)q_whole_backlog_code, flag, tqmts0x["FACTORY_DIV"].ToString(), (const char*)base_code);

		if(q_st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		tqmts0x["ST_NO"] = q_st_no;
		tqmts0x["BASE_CODE"] = base_code;
		tqmts0x.Query("ST_NO,FACTORY_DIV,BASE_CODE");
		//调用检查出钢记号是否在计划中使用的函数
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		bcls_rec_s.Tables[1].Rows.Add();
		bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = q_st_no;
		doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);
		if(doFlag != 0)
		{    
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tep0002["CODE_CLASS"] = "QMZ7";
		tep0002["CODE_DESC_4_CONTENT"] = table_name.ToUpper();
		tep0002.Query();
		//q_whole_backlog_code = tep0002["CODE"];
		if(flag == 0)
		{
			//制造标准
			if(table_name == "tqmts03")
			{
				tqmts03.Reset();
				tqmts03.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else if(table_name == "tqmts04")
			{
				tqmts04.Reset();
				tqmts04.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else if(table_name == "tqmts05")
			{
				tqmts05.Reset();
				tqmts05.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else if (table_name == "tqmts05p")
			{
				tqmts05p.Reset();
				tqmts05p.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else if(table_name == "tqmts06")
			{
				tqmts06.Reset();
				tqmts06.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else if(table_name == "tqmts07")
			{
				tqmts07.Reset();
				tqmts07.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else if(table_name == "tqmts08")
			{
				tqmts08.Reset();
				tqmts08.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else if(table_name == "tqmts0a")
			{
				tqmts0a.Reset();
				tqmts0a.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else if(table_name == "tqmts0m")
			{
				tqmts0m.Reset();
				tqmts0m.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}
			else if (table_name == "tqmts0l")
			{
				tqmts0l.Reset();
				tqmts0l.MergeFrom(bcls_rec->Tables[0].Rows[0]);
			}

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT COUNT(1) "
							 "  FROM " + table_name +
							 " WHERE ST_NO = @st_no "
							 " AND FACTORY_DIV =@factory_div"
					         " AND BASE_CODE =@base_code";
					break;
			}
			if (s_equ_no.TrimOrBlank() != " " || s_ingot_code.TrimOrBlank() != " ")
			{
				sqlstr = sqlstr + " AND EQU_NO=@equ_no AND INGOT_CODE=@ingot_code";
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no", q_st_no);
			cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
			cmd_inq.Parameters.Set("base_code", base_code);
			if (s_equ_no.TrimOrBlank() != " " || s_ingot_code.TrimOrBlank() != " ")
			{
				cmd_inq.Parameters.Set("equ_no", s_equ_no.TrimOrBlank());
				cmd_inq.Parameters.Set("ingot_code", s_ingot_code.TrimOrBlank());
			}
			v_count = cmd_inq.ExecuteScalar();
			cmd_inq.Close();
			if(v_count > 0)
			{
				sprintf(s.msg,_RES("QM00S0003880")/*出钢记号[{0}]已存在，不能再新增。*/,(const char*)q_st_no);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//赋初值,同步到制造标准管理表
			tqmts01["REC_CREATOR"] = s.userid;
			tqmts01["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts01["REC_REVISOR"] = " ";
			tqmts01["REC_REVISE_TIME"] = " ";
			tqmts01["ARCHIVE_FLAG"] = " ";
			tqmts01["VERSION"] = 1;
			tqmts01["DU_FLAG"] = " ";
			tqmts01["DU_MAKER"] = " ";
			tqmts01["DU_TIME"] = " ";
			tqmts01["IF_PASS"] = "0";
			tqmts01["IF_PLAN"] = "1";
			tqmts01["IF_MESSAGE"] = "0";
			tqmts01["ST_NO"] = q_st_no;
			tqmts01["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts01["BASE_CODE"] = base_code;
			tqmts01["WHOLE_BACKLOG_CODE"] = q_whole_backlog_code;
			tqmts01.TrimOrBlank();
			tqmts01.Insert();

			if(table_name == "tqmts03")
			{
				tqmts03["REC_CREATOR"] = s.userid;
				tqmts03["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts03["REC_REVISOR"] = " ";
				tqmts03["REC_REVISE_TIME"] = " ";
				tqmts03["ARCHIVE_FLAG"] = " ";
				tqmts03["VERSION"] = 1;
				tqmts03["DU_FLAG"] = " ";
				tqmts03["DU_MAKER"] = " ";
				tqmts03["DU_TIME"] = " ";
				tqmts03["ST_NO"] = q_st_no;
				tqmts03["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts03["BASE_CODE"] = base_code;
				tqmts03.TrimOrBlank();
				tqmts03.Insert();
			}
			else if(table_name == "tqmts04")
			{
				tqmts04["REC_CREATOR"] = s.userid;
				tqmts04["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts04["REC_REVISOR"] = " ";
				tqmts04["REC_REVISE_TIME"] = " ";
				tqmts04["ARCHIVE_FLAG"] = " ";
				tqmts04["VERSION"] = 1;
				tqmts04["DU_FLAG"] = " ";
				tqmts04["DU_MAKER"] = " ";
				tqmts04["DU_TIME"] = " ";
				tqmts04["ST_NO"] = q_st_no;
				tqmts04["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts04["BASE_CODE"] = base_code;
				tqmts04.TrimOrBlank();
				tqmts04.Insert();
			}
			else if(table_name == "tqmts05")
			{
				tqmts05["REC_CREATOR"] = s.userid;
				tqmts05["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts05["REC_REVISOR"] = " ";
				tqmts05["REC_REVISE_TIME"] = " ";
				tqmts05["ARCHIVE_FLAG"] = " ";
				tqmts05["VERSION"] = 1;
				tqmts05["DU_FLAG"] = " ";
				tqmts05["DU_MAKER"] = " ";
				tqmts05["DU_TIME"] = " ";
				tqmts05["ST_NO"] = q_st_no;
				tqmts05["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts05["BASE_CODE"] = base_code;
				tqmts05.TrimOrBlank();
				tqmts05.Insert();
			}
			else if (table_name == "tqmts05p")
			{
				tqmts05p["REC_CREATOR"] = s.userid;
				tqmts05p["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts05p["REC_REVISOR"] = " ";
				tqmts05p["REC_REVISE_TIME"] = " ";
				tqmts05p["ARCHIVE_FLAG"] = " ";
				tqmts05p["VERSION"] = 1;
				tqmts05p["DU_FLAG"] = " ";
				tqmts05p["DU_MAKER"] = " ";
				tqmts05p["DU_TIME"] = " ";
				tqmts05p["ST_NO"] = q_st_no;
				tqmts05p["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts05p["BASE_CODE"] = base_code;
				tqmts05p.TrimOrBlank();
				tqmts05p.Insert();
			}
			else if(table_name == "tqmts06")
			{
				tqmts06["REC_CREATOR"] = s.userid;
				tqmts06["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts06["REC_REVISOR"] = " ";
				tqmts06["REC_REVISE_TIME"] = " ";
				tqmts06["ARCHIVE_FLAG"] = " ";
				tqmts06["VERSION"] = 1;
				tqmts06["DU_FLAG"] = " ";
				tqmts06["DU_MAKER"] = " ";
				tqmts06["DU_TIME"] = " ";
				tqmts06["ST_NO"] = q_st_no;
				tqmts06["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts06["BASE_CODE"] = base_code;
				tqmts06.TrimOrBlank();
				tqmts06.Insert();
			}
			else if(table_name == "tqmts07")
			{
				tqmts07["REC_CREATOR"] = s.userid;
				tqmts07["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts07["REC_REVISOR"] = " ";
				tqmts07["REC_REVISE_TIME"] = " ";
				tqmts07["ARCHIVE_FLAG"] = " ";
				tqmts07["VERSION"] = 1;
				tqmts07["DU_FLAG"] = " ";
				tqmts07["DU_MAKER"] = " ";
				tqmts07["DU_TIME"] = " ";
				tqmts07["ST_NO"] = q_st_no;
				tqmts07["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts07["BASE_CODE"] = base_code;
				tqmts07.TrimOrBlank();
				tqmts07.Insert();
			}
			else if(table_name == "tqmts08")
			{
				tqmts08["REC_CREATOR"] = s.userid;
				tqmts08["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts08["REC_REVISOR"] = " ";
				tqmts08["REC_REVISE_TIME"] = " ";
				tqmts08["ARCHIVE_FLAG"] = " ";
				tqmts08["VERSION"] = 1;
				tqmts08["DU_FLAG"] = " ";
				tqmts08["DU_MAKER"] = " ";
				tqmts08["DU_TIME"] = " ";
				tqmts08["ST_NO"] = q_st_no;
				tqmts08["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts08["BASE_CODE"] = base_code;
				tqmts08.TrimOrBlank();
				tqmts08.Insert();
				Log::Trace("", "","33333");
			}
			else if(table_name == "tqmts0a")
			{
				tqmts0a["REC_CREATOR"] = s.userid;
				tqmts0a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts0a["REC_REVISOR"] = " ";
				tqmts0a["REC_REVISE_TIME"] = " ";
				tqmts0a["ARCHIVE_FLAG"] = " ";
				tqmts0a["VERSION"] = 1;
				tqmts0a["DU_FLAG"] = " ";
				tqmts0a["DU_MAKER"] = " ";
				tqmts0a["DU_TIME"] = " ";
				tqmts0a["ST_NO"] = q_st_no;
				tqmts0a["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts0a["BASE_CODE"] = base_code;
				tqmts0a.TrimOrBlank();
				tqmts0a.Insert();
			}
				else if(table_name == "tqmts0m")
			{
				tqmts0m["REC_CREATOR"] = s.userid;
				tqmts0m["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts0m["REC_REVISOR"] = " ";
				tqmts0m["REC_REVISE_TIME"] = " ";
				tqmts0m["ARCHIVE_FLAG"] = " ";
				tqmts0m["VERSION"] = 1;
				tqmts0m["DU_FLAG"] = " ";
				tqmts0m["DU_MAKER"] = " ";
				tqmts0m["DU_TIME"] = " ";
				tqmts0m["ST_NO"] = q_st_no;
				tqmts0m["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				tqmts0m["BASE_CODE"] = base_code;
				tqmts0m.TrimOrBlank();
				tqmts0m.Insert();
				Log::Trace("", "","44444");
			}
				else if (table_name == "tqmts0l")
			{
			tqmts0l["REC_CREATOR"] = s.userid;
			tqmts0l["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts0l["REC_REVISOR"] = " ";
			tqmts0l["REC_REVISE_TIME"] = " ";
			tqmts0l["ARCHIVE_FLAG"] = " ";
			tqmts0l["VERSION"] = 1;
			tqmts0l["DU_FLAG"] = " ";
			tqmts0l["DU_MAKER"] = " ";
			tqmts0l["DU_TIME"] = " ";
			tqmts0l["ST_NO"] = q_st_no;
			tqmts0l["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts0l["BASE_CODE"] = base_code;
			tqmts0l.TrimOrBlank();
			tqmts0l.Insert();
			}

		}
		//else if(flag == 1)
		//{
		//	//制造标准成分标准
		//	for (i = 0; i <bcls_rec->Tables[1].Rows.get_Count(); i++ )
		//	{
		//		//取得单行传入信息
		//		tqmts02.Reset();
		//		tqmts02.MergeFrom(bcls_rec->Tables[1].Rows[i]);

		//		if(tqmts02["ELM_CODE"].ToString().TrimOrBlank() == " ")
		//		{
		//			sprintf(s.msg,_RES("QM00S0004168")/*元素代码不能为空。*/);
		//			throw CApplicationException(-1, s.msg, s.svc_name);
		//		}
		//		if(tqmts02["SMELT_CHEMI_FLAG"].ToString().TrimOrBlank() == " ")
		//		{
		//			sprintf(s.msg, "取样指示不能为空");
		//		//	sprintf(s.msg,"判定及打质保书标记不能为空");
		//			throw CApplicationException(-1, s.msg, s.svc_name);
		//		}

		//		tqmts02["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		//		tqmts02["ST_NO"] = q_st_no;
		//		tqmts02["BASE_CODE"] = base_code;
		//		tqmts02["WHOLE_BACKLOG_CODE"] = q_whole_backlog_code;

		//		switch(conn->DatabaseKind)
		//		{
		//			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//			case DB_KIND_MSSQL:				// MS SQL Server数据库
		//			case DB_KIND_ORACLE:	        // Oracle 数据库
		//			default:						// 所有数据库适用，通用SQL语句
		//				sqlstr = "SELECT COUNT(1) "
		//						 "  FROM TQMTS02 "
		//						 " WHERE FACTORY_DIV  = @factory_div "
		//						 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
		//						 "   AND ST_NO = @st_no "
		//					     "   AND BASE_CODE = @base_code "
		//						 "   AND ELM_CODE = @elm_code ";
		//				break;
		//		}
		//		cmd_inq.SetCommandText(sqlstr);
		//		cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
		//		cmd_inq.Parameters.Set("factory_div", tqmts02["FACTORY_DIV"].ToString());
		//		cmd_inq.Parameters.Set("base_code", base_code);
		//		cmd_inq.Parameters.Set("whole_backlog_code", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
		//		cmd_inq.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString());
		//		v_count = cmd_inq.ExecuteScalar();	//ExecuteScalar只返回查询结果集中的第一行的第一列，忽略额外的列或行，适用于单纯计算COUNT,SUM,MAX等的sql语句。
		//		cmd_inq.Close();
		//		if(v_count > 0)
		//		{
		//			sprintf(s.msg,_RES("QM00S0003880")/*出钢记号[{0}]已存在，不能再新增。*/,(const char*)tqmts02["ST_NO"].ToString());
		//			throw CApplicationException(-1, s.msg, log.Location);
		//		}

		//		if(tqmts02["MAIN_MIN"].ToDecimal()  >tqmts02["MAIN_MAX"].ToDecimal() || tqmts02["MAIN_AIM"].ToDecimal() >tqmts02["MAIN_MAX"].ToDecimal() || tqmts02["MAIN_MIN"].ToDecimal() >tqmts02["MAIN_AIM"].ToDecimal())
		//		{
		//			CFormattable arguments[] = {(const char*)tqmts02["ST_NO"].ToString(),(const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString(),(const char*)tqmts02["ELM_NAME"].ToString()};
		//			CMessageFormat::Format(s.msg, _RES("QM00S0004178")/*出钢记号[{0}]工序[{1}]对应的[{2}]元素主试成分数据倒置。*/, arguments, 3);
		//			throw CApplicationException(-1, s.msg, log.Location);
		//		}
		//		if(tqmts02["SPE_MIN"].ToDecimal()  >tqmts02["SPE_MAX"].ToDecimal() )
		//		{
		//			CFormattable arguments[] = {(const char*)tqmts02["ST_NO"].ToString(),(const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString(),(const char*)tqmts02["ELM_NAME"].ToString()};
		//			CMessageFormat::Format(s.msg, _RES("QM00S0004001")/*出钢记号[{0}]工序[{1}]对应的[{2}]元素特采成分数据倒置。*/, arguments, 3);
		//			throw CApplicationException(-1, s.msg, log.Location);
		//		}

		//		/******************** 赋初值 *************************/
		//		tqmts02["REC_CREATOR"] = s.userid;
		//		tqmts02["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//		tqmts02["REC_REVISOR"] = " ";
		//		tqmts02["REC_REVISE_TIME"] = " ";
		//		tqmts02["ARCHIVE_FLAG"] = " ";
		//		tqmts02["VERSION"] = 1;
		//		tqmts02["DU_FLAG"] = " ";
		//		tqmts02["DU_MAKER"] = " ";
		//		tqmts02["DU_TIME"] = " ";

		//		//获取工序顺序号
		//		switch(conn->DatabaseKind)
		//		{
		//			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//			case DB_KIND_MSSQL:				// MS SQL Server数据库
		//			case DB_KIND_ORACLE:	        // Oracle 数据库
		//			default:						// 所有数据库适用，通用SQL语句
		//				sqlstr = "SELECT WHOLE_BACKLOG_SEQ "
		//						 "  FROM TQMTS01 "
		//						 " WHERE FACTORY_DIV  = @factory_div "
		//						 "   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
		//					     "   AND BASE_CODE = @base_code "
		//						 "   AND ST_NO = @st_no ";
		//				break;
		//		}
		//		cmd_inq.SetCommandText(sqlstr);
		//		cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
		//		cmd_inq.Parameters.Set("factory_div", tqmts02["FACTORY_DIV"].ToString());
		//		cmd_inq.Parameters.Set("base_code", base_code);
		//		cmd_inq.Parameters.Set("whole_backlog_code", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
		//		cmd_inq.ExecuteReader();
		//		if(cmd_inq.Read())
		//		{
		//			tqmts02["WHOLE_BACKLOG_SEQ"] = cmd_inq.GetInt32(1);
		//		}
		//		cmd_inq.Close();

		//		//获取元素顺序、元素单位
		//		switch(conn->DatabaseKind)
		//		{
		//			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//			case DB_KIND_MSSQL:				// MS SQL Server数据库
		//			case DB_KIND_ORACLE:	        // Oracle 数据库
		//			default:						// 所有数据库适用，通用SQL语句
		//				sqlstr = "SELECT * "
		//						 "  FROM TEP0002 "
		//						 " WHERE CODE_CLASS = 'QMYS' "
		//						 "   AND CODE = @elm_code ";
		//				break;
		//		}
		//		cmd_inq.SetCommandText(sqlstr);
		//		cmd_inq.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString());
		//		cmd_inq.ExecuteReader();
		//		if(cmd_inq.Read())
		//		{
		//			cmd_inq.Fetch(tep0002);
		//			tqmts02["ELM_POS"] = CDecimal::Parse(tep0002["CODE_DESC_2_CONTENT"].ToString());//得到元素顺序
		//			tqmts02["ELM_UNIT"] = "%";
		//		}
		//		cmd_inq.Close();

		//		tqmts02.TrimOrBlank();
		//		tqmts02.Insert();	
		//	}
		//}

		//修改工艺卡确认－生效标记
		//switch(conn->DatabaseKind)
		//{
		//	case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		//	case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		//	case DB_KIND_MSSQL:				// MS SQL Server数据库
		//	case DB_KIND_ORACLE:	        // Oracle 数据库
		//	default:						// 所有数据库适用，通用SQL语句
		//		sqlstr = "SELECT COUNT(1) "
		//				 "  FROM TQMTS01 "
		//				 " WHERE ST_NO = @st_no"
		//				 " AND WHOLE_BACKLOG_CODE = @whole_backlog_code"
  //                       " AND BASE_CODE = @base_code "
		//				 " AND FACTORY_DIV  = @factory_div";
		//		break;
		//}
		//cmd_inq.SetCommandText(sqlstr);
		//cmd_inq.Parameters.Set("st_no", q_st_no);				
		//cmd_inq.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());	
		//cmd_inq.Parameters.Set("base_code", base_code);
		//cmd_inq.Parameters.Set("whole_backlog_code", q_whole_backlog_code);
		//v_count = cmd_inq.ExecuteScalar();
		//cmd_inq.Close();
		//if (v_count > 0 )
		//{
		//	tqmts01["IF_PASS"] = "0";
		//	tqmts01["IF_MESSAGE"] = " ";
		//	tqmts01["MESSAGE_TIME"] = " ";
		//	tqmts01["FIN_CONFM_MAKER"] = " ";
		//	tqmts01["FIN_CONFM_TIME"] = " ";
		//	tqmts01["RES_CODE"] = " ";
		//	tqmts01["REC_REVISOR"] = s.userid;
		//	tqmts01["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		//	tqmts01["ST_NO"] = q_st_no;
		//	tqmts01["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
		//	tqmts01["BASE_CODE"] = base_code;
		//	tqmts01["WHOLE_BACKLOG_CODE"] = q_whole_backlog_code;
		//	tqmts01.Update("IF_PASS,IF_MESSAGE,MESSAGE_TIME,FIN_CONFM_MAKER,FIN_CONFM_TIME,RES_CODE,REC_REVISOR,REC_REVISE_TIME",  //修改字段项
		//					"ST_NO,WHOLE_BACKLOG_CODE,FACTORY_DIV,BASE_CODE"); //条件字段项
		//}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", "","error=[{0}]", (const char*)str );

		strncpy(s.sysmsg, (const char*)str, 399);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
