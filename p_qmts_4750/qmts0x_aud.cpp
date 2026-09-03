/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-11
Description: 审核工艺卡
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 审核工艺卡
/// <para>
/// 1.审核工艺卡和对应的成分标准信息
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡表);
//                  TQMTS02(工序成分标准表)                </para>
/// <para>主调用函数：前台QMTS0X画面的F8(审核)调用。       </para>
/// </summary>
/// <param name="ST_NO"> 出钢记号                          </param>
===========================================================</remark>*/
//int f_cm_0020q1_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn); ////发送制造标准(如果制造标准在MMS上编制)
int f_qm002001_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn); ////发送工艺卡和成分
// service入口
BM2F_ENTERACE(qmts0x_aud)

int f_qmts0x_aud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	int elm_count = 0;
	int colno2 = 0;
	CDecimal i_count = 0;
	CString  s_table_ename = "";
	CString s_idx_table_ename = "";
	CString s_tc_no = "";
	CString s_refine_route_code = "";
	CString base_code = " ";

	CString  insert_flag = "";
	CString  columName = "";
	CString  datetime = "";
	CString  sql_ins = "";
	CString  sql_value = "";
	CString  s_idx_no = "";
	CString  sql_upd = "";

	CString  ts0x_colName = "";

	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");
	CModel hqmts0x("HQMTS0X");

	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");

	CDbCommand cmd_inq_tep02(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_02(conn);
	CDbCommand cmd_ins(conn);
	CDbCommand cmd_del(conn);
	CDbCommand cmd_upd(conn);
	CString sqlstr("");
	CString sqlstr_tep02("");


	EIClass bcls_rec_ts0x;

	EIClass bcls_rec_q1;
	EIClass bcls_ret_q1;
	bcls_rec_q1.Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
	bcls_rec_q1.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_q1.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_q1.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	bcls_rec_q1.Tables[0].Rows.Add();

	if (!bcls_rec->Tables.Contains("TQMTSXX"))////目的基表（制造标准结果表）
	{
		bcls_rec->Tables.Add("TQMTSXX");
	}
	if (!bcls_rec->Tables.Contains("TQMTMXX"))////源基表（制造标准索引表）
	{
		bcls_rec->Tables.Add("TQMTMXX");
	}
	try
	{
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		tqmts0x["BASE_CODE"] = base_code;
		tqmts02["BASE_CODE"] = base_code;
		tqmts0x["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		/////字符型DB2数据库是个null, oracle是个空格，update by yiling 20160620,主键不好改成动态SQL查询
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容	
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
			break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
			break;
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank();
			break;
		}
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", "", "qmts0x_aud IN:---ST_NO = [{0}] tqmts0x.FACTORY_DIV[{1}]", (const char*)tqmts0x["ST_NO"].ToString(), tqmts0x["FACTORY_DIV"].ToString());
		//厂别区分可能会有多个不能写死，审核必须所有厂别一起审核
		//tqmts0x["FACTORY_DIV"] = "A";

		//校验出钢记号是否已下发——add by 冯晓轶 2012-04-26
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT MAX(VALID_FLAG) "
				"   FROM TQMTS0X "						   
				"  WHERE ST_NO = @tqmts0x.ST_NO "
				" AND FACTORY_DIV  = @tqmts0x.FACTORY_DIV ";
			if (base_code.Trim() != "")
			{
				sqlstr += "    AND base_code = @base_code ";
			}
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			tqmts0x["VALID_FLAG"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		if (tqmts0x["VALID_FLAG"].ToString() == "2" && tqmts0x["IDX_NO_ELM"].ToString().Trim() == "")
		{
			strcpy(s.msg,_RES("QM00S0005920")/*该出钢记号已下发，不可做审核。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT IDX_NO_ELM "
				"   FROM TQMTS0X "
				"  WHERE ST_NO = @tqmts0x.ST_NO "
				" AND FACTORY_DIV = @tqmts0x.FACTORY_DIV ";
			if (base_code.Trim() != "")
			{
				sqlstr += "    AND base_code = @base_code ";
			}
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts0x["IDX_NO_ELM"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		Log::Trace("", "", "1111tqmts0x.IDX_NO_ELM = [{0}]", (const char*)tqmts0x["IDX_NO_ELM"].ToString());

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS0X "
				"  WHERE ST_NO = @tqmts0x.ST_NO "
				" AND FACTORY_DIV = @tqmts0x.FACTORY_DIV ";
			if (base_code.Trim() != "")
			{
				sqlstr += "    AND base_code = @base_code ";
			}
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.ExecuteReader();
		if(cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts0x);
			cmd_inq.Fetch(hqmts0x);

			s_refine_route_code = tqmts0x["REFINE_ROUTE_CODE"];
			i_count = 0;
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = " SELECT COUNT(1) "
					"   FROM TQMTS02 "
					"  WHERE FACTORY_DIV = @tqmts0x.FACTORY_DIV "
					"    AND ST_NO = @tqmts0x.ST_NO "
					"    AND WHOLE_BACKLOG_CODE = 'G' ";
				if (base_code.Trim() != "")
				{
					sqlstr += "    AND base_code = @base_code ";
				}
				break;
			}
			cmd_inq_02.SetCommandText(sqlstr);
			cmd_inq_02.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
			cmd_inq_02.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
			cmd_inq_02.Parameters.Set("base_code", base_code);
			i_count = cmd_inq_02.ExecuteScalar();
			cmd_inq_02.Close();
			if (i_count <= 0 && tqmts0x["IDX_NO_ELM"].ToString().Trim() =="")
			{
				CFormattable arguments[] = { tqmts0x["ST_NO"].ToString(),tqmts0x["FACTORY_DIV"].ToString() };// 定义参数列表的数组
				CMessageFormat::Format(s.msg, "出钢记号[{0}]厂别[{1}]没有维护成分标准", arguments, 2);//格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tqmts0x["SMELT_DIV"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, _RES("QM00S0005922")/*审核失败：该出钢记号工艺卡缺少冶炼区分。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tqmts0x["REFINE_ROUTE_CODE"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, _RES("QM00S0005923")/*审核失败：该出钢记号工艺卡缺少精炼路径。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tqmts0x["LABEL1"].ToString().TrimOrBlank() == " "
				&& tqmts0x["LABEL2"].ToString().TrimOrBlank() == " "
				&& tqmts0x["LABEL3"].ToString().TrimOrBlank() == " "
				&& tqmts0x["LABEL4"].ToString().TrimOrBlank() == " "
				&& tqmts0x["LABEL5"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, _RES("QM00S0005924")/*审核失败：该出钢记号工艺卡缺少适用牌号。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts0x["VALID_FLAG"] = "1";
			tqmts0x["CHECK_MAKER"] = s.userid;
			tqmts0x["CHECK_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			if (base_code.Trim() != "")
			{
				tqmts0x.Update("VALID_FLAG,CHECK_TIME,CHECK_MAKER",
					"FACTORY_DIV,ST_NO,BASE_CODE");
			}
			else
			{
				tqmts0x.Update("VALID_FLAG,CHECK_TIME,CHECK_MAKER",
					"FACTORY_DIV,ST_NO");
			}
			//审核后写入履历
			hqmts0x["DU_FLAG"] = "U";
			hqmts0x["DU_MAKER"] = s.userid;
			hqmts0x["DU_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			hqmts0x.TrimOrBlank();
			hqmts0x.Insert();
		}
		cmd_inq.Close();

		//////////工艺卡挂索引形式，审核时按索引复制到不同的制造标准和成分表里update by yiling 20160406
		Log::Trace("", "", "qmts0x_aud IN:---tqmts0x.IDX_NO_ELM = [{0}]", (const char*)tqmts0x["IDX_NO_ELM"].ToString());

#ifdef _SYS_MMS   /*是MMS系统时，发送电文到PES*/
		Log::Trace("", "", "主工艺卡加成分电文SEND TC[002001]"); ////先发这条电文，L3根据工艺卡的生效标记做电文接收处理
		doFlag = f_qm002001_snd(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
#endif
//		if (tqmts0x["IDX_NO_ELM"].ToString().Trim() != "")  /////成分索引不为空则为新的挂索引模式，开始
//		{
//			/******************** 赋初值 *************************/
//			tqmts02["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
//			tqmts02["ST_NO"] = tqmts0x["ST_NO"];
//			tqmts02["BASE_CODE"] = base_code;
//			tqmts02.Delete("FACTORY_DIV,ST_NO,BASE_CODE");
//			Log::Trace("", "", "s_refine_route_code= [{0}]", (const char*)s_refine_route_code);
//
//			///////////校验成分及制造标准是否挂完整update by yiling 20170110
//			for (int kk=0; kk < s_refine_route_code.GetLength();kk++)
//			{
//				s_refine_route_code = s_refine_route_code.Substring(kk,1);
//				Log::Trace("", "", "s_refine_route_code= [{0}]", (const char*)s_refine_route_code);
//				switch (conn->DatabaseKind)
//				{
//				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
//				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
//				case DB_KIND_MSSQL:	        // MS SQL Server数据库
//				case DB_KIND_ORACLE:	        // Oracle 数据库
//				default:
//
//
//					sqlstr =" select count(*) from tqmtms0 "
//				           	" where IDX_NO	 	 =@tqmts0x.IDX_NO_ELM"
//							" and whole_backlog_code =@s_refine_route_code";
//					if (base_code.Trim() != "")
//					{
//						sqlstr += "    AND base_code = @base_code ";
//					}
//					break;
//				}
//				cmd_inq.SetCommandText(sqlstr);
//				cmd_inq.Parameters.Set("tqmts0x.IDX_NO_ELM", tqmts0x["IDX_NO_ELM"].ToString());
//				cmd_inq.Parameters.Set("s_refine_route_code", s_refine_route_code);
//				cmd_inq.Parameters.Set("base_code", base_code);
//				i_count = cmd_inq.ExecuteScalar();
//				if (i_count <= 0)
//				{
//					CFormattable arguments[] = { tqmts0x["IDX_NO_ELM"].ToString(),s_refine_route_code };// 定义参数列表的数组
//					CMessageFormat::Format(s.msg, "成分索引[{0}]]工序[{1}]没有维护成分标准", arguments, 3);//格式化字符串
//					throw CApplicationException(-1, s.msg, log.Location);
//				}
//
//				if (s_refine_route_code == "L" && tqmts0x["IDX_NO_06"].ToString().Trim()=="")
//				{
//					CFormattable arguments[] = { tqmts0x["ST_NO"].ToString(), tqmts0x["FACTORY_DIV"].ToString() };// 定义参数列表的数组
//					CMessageFormat::Format(s.msg, "出钢记号[{0}]厂别[{1}]LF没有挂制造标准索引", arguments, 2);//格式化字符串
//					throw CApplicationException(-1, s.msg, log.Location);
//				}
//				if (s_refine_route_code == "R" && tqmts0x["IDX_NO_08"].ToString().Trim() == "")
//				{
//					CFormattable arguments[] = { tqmts0x["ST_NO"].ToString(), tqmts0x["FACTORY_DIV"].ToString()};// 定义参数列表的数组
//					CMessageFormat::Format(s.msg, "出钢记号[{0}]厂别[{1}]RH没有挂制造标准索引", arguments, 2);//格式化字符串
//					throw CApplicationException(-1, s.msg, log.Location);
//				}
//				if (s_refine_route_code == "V" && tqmts0x["IDX_NO_10"].ToString().Trim() == "")
//				{
//					CFormattable arguments[] = { tqmts0x["ST_NO"].ToString(), tqmts0x["FACTORY_DIV"].ToString() };// 定义参数列表的数组
//					CMessageFormat::Format(s.msg, "出钢记号[{0}]厂别[{1}]VD/VOD没有挂制造标准索引", arguments, 2);//格式化字符串
//					throw CApplicationException(-1, s.msg, log.Location);
//				}
//			}
//			switch (conn->DatabaseKind)
//			{
//			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
//			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
//			case DB_KIND_MSSQL:	        // MS SQL Server数据库
//			case DB_KIND_ORACLE:	        // Oracle 数据库
//			default:
//
//
//				sqlstr = "SELECT *  "
//					"  FROM   tqmtms0   "
//					"  WHERE  IDX_NO	 	 =@tqmts0x.IDX_NO_ELM ";
//				if (base_code.Trim() != "")
//				{
//					sqlstr += "    AND base_code = @base_code ";
//				}
//				break;
//			}
//
//			cmd_inq.SetCommandText(sqlstr);
//			cmd_inq.Parameters.Set("tqmts0x.IDX_NO_ELM", tqmts0x["IDX_NO_ELM"].ToString());
//			cmd_inq.Parameters.Set("base_code", base_code);
//			cmd_inq.ExecuteReader();
//			while (cmd_inq.Read())
//			{
//
//				cmd_inq.Fetch(tqmts02);
//				if (tqmts02["WHOLE_BACKLOG_CODE"].ToString() == "C")
//				{
//					elm_count++;
//				}
//				tqmts02["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
//				tqmts02["ST_NO"] = tqmts0x["ST_NO"];
//				tqmts02["WHOLE_BACKLOG_SEQ"] = 0;
//				tqmts02["REC_CREATOR"] = s.userid;   /* 记录创建责任者 */
//				tqmts02["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");          		     /* 记录创建时刻 */
//				tqmts02["REC_REVISOR"] = " ";   /* 记录修改责任者 */
//				tqmts02["REC_REVISE_TIME"] = " ";   /* 记录修改时刻 */
//				tqmts02["ARCHIVE_FLAG"] = " ";   /* 归档标记 */
//				if (base_code.Trim() != "")
//				{
//					tqmts02["BASE_CODE"] = base_code;
//				}
//				tqmts02.Insert();
//			}
//			cmd_inq.Close();
//			if (elm_count == 0)
//			{
//				CFormattable arguments[] = { tqmts0x["IDX_NO_ELM"].ToString() };
//				CMessageFormat::Format(s.msg, "索引号[{0}]连铸工序上的成分没有维护。", arguments, 1);
//				Log::Trace("", "", "没有满足条件的记录!");
//				throw CApplicationException(-1, s.msg, s.svc_name);
//			}
//			Log::Trace("", "", "写各制造标准的表内数据。");
//
//			switch (conn->DatabaseKind)
//			{
//			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
//			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
//			case DB_KIND_MSSQL:	        // MS SQL Server数据库
//			case DB_KIND_ORACLE:	        // Oracle 数据库
//			default:
//
//				sqlstr_tep02 = "SELECT * FROM TEP0002 WHERE CODE_CLASS='QMZ7'"
//					;
//				break;
//			}
//			cmd_inq_tep02.SetCommandText(sqlstr_tep02);
//			cmd_inq_tep02.ExecuteReader();
//			while (cmd_inq_tep02.Read())
//			{
//				insert_flag = "";
//				cmd_inq_tep02.Fetch(tep0002);
//				s_idx_table_ename = tep0002["CODE_DESC_3_CONTENT"];  ////制造标准索引表
//				s_table_ename = tep0002["CODE_DESC_4_CONTENT"];  ////制造标准表
//				s_tc_no = tep0002["CODE_DESC_5_CONTENT"];  ////电文号
//				switch (conn->DatabaseKind)
//				{
//				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
//				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
//				case DB_KIND_MSSQL:	        // MS SQL Server数据库
//				case DB_KIND_ORACLE:	        // Oracle 数据库
//				default:
//
//					//开始拼接Delete
//					sqlstr = " DELETE "
//						+ s_table_ename +
//						" WHERE"
//						" ST_NO   = @tqmts0x.ST_NO"
//						" AND FACTORY_DIV   = @tqmts0x.FACTORY_DIV"
//						;
//					if (base_code.Trim() != "")
//					{
//						sqlstr += "    AND base_code = @base_code ";
//					}
//					break;
//				}
//				cmd_del.SetCommandText(sqlstr);
//				cmd_del.Parameters.Set("s_table_ename", s_table_ename);
//				cmd_del.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
//				cmd_del.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
//				cmd_del.Parameters.Set("base_code", base_code);
//				cmd_del.ExecuteNonQuery();
//				if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_01" && tqmts0x["IDX_NO_01"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_01"];
//					insert_flag = "I";
//				}
//				else if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_02" && tqmts0x["IDX_NO_02"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_02"];
//					insert_flag = "I";
//				}
//				else if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_03" && tqmts0x["IDX_NO_03"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_03"];
//					insert_flag = "I";
//				}
//				else if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_04" && tqmts0x["IDX_NO_04"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_04"];
//					insert_flag = "I";
//				}
//				else if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_05" && tqmts0x["IDX_NO_05"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_05"];
//					insert_flag = "I";
//				}
//				else if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_06" && tqmts0x["IDX_NO_06"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_06"];
//					insert_flag = "I";
//				}
//				else if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_07" && tqmts0x["IDX_NO_07"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_07"];
//					insert_flag = "I";
//				}
//				else if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_08" && tqmts0x["IDX_NO_08"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_08"];
//					insert_flag = "I";
//				}
//				else if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_09" && tqmts0x["IDX_NO_09"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_09"];
//					insert_flag = "I";
//				}
//				else if (tep0002["CODE_DESC_2_CONTENT"].ToString() == "IDX_NO_10" && tqmts0x["IDX_NO_10"].ToString().Trim() != "")
//				{
//					s_idx_no = tqmts0x["IDX_NO_10"];
//					insert_flag = "I";
//				}
//				if (insert_flag == "I")  //////要增加此制造标准
//				{
//					Log::Trace("", "", "复制制造标准源表[{0}]目的表[{1}]。", s_idx_table_ename, s_table_ename);
//
//					switch (conn->DatabaseKind)
//					{
//					case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
//						sqlstr = " SELECT *  FROM  "
//							+ s_table_ename +
//							" FETCH FIRST 1 ROWS ONLY"
//							;
//						break;
//					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
//					case DB_KIND_MSSQL:	        // MS SQL Server数据库
//					case DB_KIND_ORACLE:	        // Oracle 数据库
//					default:
//
//
//						sqlstr = " SELECT *  FROM  "
//							+ s_table_ename +
//							" WHERE ROWNUM = 1 "
//							;
//						break;
//					}
//
//					cmd_inq.SetCommandText(sqlstr);
//					cmd_inq.ExecuteQuery(bcls_rec->Tables["TQMTSXX"]);
//					bcls_rec->Tables["TQMTSXX"].Rows.Clear();//获取要新增表的字段名
//
//					switch (conn->DatabaseKind)
//					{
//					case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
//					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
//					case DB_KIND_MSSQL:	        // MS SQL Server数据库
//					case DB_KIND_ORACLE:	        // Oracle 数据库
//					default:
//
//
//						sqlstr = " SELECT *  FROM  "
//							+ s_idx_table_ename +
//							" WHERE IDX_NO =@s_idx_no"
//							;
//						break;
//					}
//
//					cmd_inq.SetCommandText(sqlstr);
//					cmd_inq.Parameters.Set("s_idx_no", s_idx_no);
//					cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
//					cmd_inq.ExecuteQuery(bcls_rec->Tables["TQMTMXX"]);
//					if (bcls_rec->Tables["TQMTMXX"].Rows.get_Count() == 0)
//					{
//						CFormattable arguments[] = { s_idx_no, s_idx_table_ename };
//						CMessageFormat::Format(s.msg, "索引号[{0}]查询表[{1}]无记录", arguments,2);
//						Log::Trace("", "", "没有满足条件的记录!");
//						throw CApplicationException(-1, s.msg, s.svc_name);
//					}
//					//////更新TQMTS0X主表的字段，因为很多地方读的是TQMTS0X开始,如果索引上挂了，就从索引上读下来。
//					//改成比对TQMTS0X与各个炼钢工序索引基表的字段，字段相同就更新，update by sunyutian 20160927
//					tqmts0x.MergeTo(bcls_rec_ts0x.Tables[0], false);
//					sql_upd = "UPDATE TQMTS0X SET ";
//					for (int colno = 1; colno <= bcls_rec->Tables["TQMTMXX"].Columns.get_Count(); colno++)
//					{
//						ts0x_colName = bcls_rec->Tables["TQMTMXX"].Columns[colno - 1].get_ColumnName().Trim();
//						Log::Trace("", "", "ts0x_colName[{0}]", ts0x_colName);
//						if (bcls_rec_ts0x.Tables[0].Columns.Contains(ts0x_colName)
//							&& bcls_rec->Tables["TQMTMXX"].Rows[0][ts0x_colName].ToString().Trim() != ""
//							&& ts0x_colName != "FACTORY_DIV"
//							&& ts0x_colName != "ST_NO"
//							&& ts0x_colName != "REC_CREATOR"
//							&& ts0x_colName != "REC_CREATE_TIME"
//							&& ts0x_colName != "REC_REVISOR"
//							&& ts0x_colName != "REC_REVISE_TIME"
//							&& ts0x_colName != "ARCHIVE_FLAG"
//							&& ts0x_colName != "DU_FLAG"
//							&& ts0x_colName != "DU_MAKER"
//							&& ts0x_colName != "DU_TIME"
//							&& ts0x_colName != "VERSION"
//							&& ts0x_colName != "ARCHIVE_STAMP_NO"
//							&& ts0x_colName != "COMPANY_CODE"
//							&& ts0x_colName != "COMPANY_NAME")
//						{
//							sql_upd += ts0x_colName + " = '" + (CString)bcls_rec->Tables["TQMTMXX"].Rows[0][ts0x_colName] + "',";
//						}
//						
//					}
//					Log::Trace("", "", "sql_upd[{0}]", sql_upd);
//					if (sql_upd != "UPDATE TQMTS0X SET ")
//					{
//						sql_upd +=  " DU_MAKER			= @du_maker ,"
//									" DU_TIME			= @du_time ,"
//									" REC_REVISOR		= @du_maker ,"
//									" REC_REVISE_TIME	= @du_time ";
//						sql_upd +=  " where ST_NO		= @st_no "
//							        "   and FACTORY_DIV =@factory_div";
//						if (base_code.Trim() != "")
//						{
//							sql_upd += "    AND base_code = @base_code ";
//						}
//						Log::Trace("", "", "sql_upd2[{0}]", sql_upd);
//						cmd_upd.SetCommandText(sql_upd);
//						cmd_upd.Parameters.Set("st_no", tqmts0x["ST_NO"].ToString());
//						cmd_upd.Parameters.Set("factory_div", tqmts0x["FACTORY_DIV"].ToString());
//						cmd_upd.Parameters.Set("du_maker", s.userid);
//						cmd_upd.Parameters.Set("du_time", datetime);
//						cmd_upd.Parameters.Set("base_code", base_code);
//						cmd_upd.ExecuteNonQuery();
//					}
//					//////更新TQMTS0X主表的字段，因为很多地方读的是TQMTS0X结束
//
//					//开始拼接insert
//					sql_ins = " INSERT INTO "
//						+ s_table_ename +
//						" ( ";
//
//					//取得制造标准表列信息 
//					for (int iCol = 1; iCol <= bcls_rec->Tables["TQMTSXX"].Columns.get_Count(); iCol++)
//					{
//						columName = bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName().Trim();
//						//Log::Trace("", "", "columName[{0}]", (const char*)columName);
//						if (bcls_rec->Tables["TQMTMXX"].Columns.Contains(columName))
//						{
//							sql_ins += bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName();
//						}
//						else if (columName == "ST_NO")
//						{
//							sql_ins += bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName();
//						}
//						if (iCol == bcls_rec->Tables["TQMTSXX"].Columns.get_Count())//最后一列
//						{
//							if (sql_ins.Substring(sql_ins.GetLength() - 2, 2) == ", ")
//							{
//								sql_ins = sql_ins.Substring(0, sql_ins.GetLength() - 2);
//							}
//							sql_ins += " ) ";
//						}
//						else if (bcls_rec->Tables["TQMTMXX"].Columns.Contains(columName) || columName == "ST_NO")
//						{
//							sql_ins += ", ";
//						}
//
//					}
//					Log::Trace("", "", "/* 新增语句头 */[{0}]", (const char*)sql_ins);
//
//					//取得单行传入信息 
//
//					sql_value = "";
//					for (int iCol = 1; iCol <= bcls_rec->Tables["TQMTSXX"].Columns.get_Count(); iCol++)
//					{
//						//Log::Trace("", "", "TQMTSXX.Columns[{0}]", (const char*)bcls_rec->Tables[0].Columns[iCol - 1].get_ColumnName());
//
//						if (iCol == 1)
//							sql_value += " values( ";
//						///清空字段的值 //////////
//						if (bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName() == "REC_REVISOR")
//						{
//							bcls_rec->Tables["TQMTMXX"].Rows[0]["REC_REVISOR"] = " "/* 记录修改责任者 */;
//						}
//						else if (bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName() == "REC_REVISE_TIME")
//						{
//							bcls_rec->Tables["TQMTMXX"].Rows[0]["REC_REVISE_TIME"] = " ";/* 记录修改时刻 */
//						}
//
//						///清空字段的值 //////////
//						columName = bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName().Trim();
//						/*Log::Trace("", "", "columName[{0}]", (const char*)columName);*/
//
//						//更新的字段
//						if (bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName() == "REC_CREATOR")
//							sql_value += "@s.userid";
//						else if (bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName() == "REC_CREATE_TIME")
//							sql_value += "@datetime";
//						else if (bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName() == "ST_NO")
//							sql_value += "@tqmts0x.ST_NO";
//						else if (bcls_rec->Tables["TQMTSXX"].Columns[iCol - 1].get_ColumnName() == "FACTORY_DIV")
//							sql_value += "@tqmts0x.FACTORY_DIV"; ///update by yiling 20170602 TQMTMSXX表里的厂别不维护，取TQMTS0X的厂别。
//						else
//						{
//							if (bcls_rec->Tables["TQMTMXX"].Columns.Contains(columName))
//							{
//								{
//									sql_value += "'" + bcls_rec->Tables["TQMTMXX"].Rows[0][columName].ToString().TrimOrBlank() + "'";
//								}
//							}
//						}
//						if (iCol == bcls_rec->Tables["TQMTSXX"].Columns.get_Count())//最后一列
//						{
//							if (sql_value.Substring(sql_value.GetLength() - 2, 2) == ", ")
//							{
//								sql_value = sql_value.Substring(0, sql_value.GetLength() - 2);
//							}
//							sql_value += ") ";
//						}
//						else if (bcls_rec->Tables["TQMTMXX"].Columns.Contains(columName) || columName == "ST_NO")
//						{
//							sql_value += ", ";
//						}
//					}
//					Log::Trace("", "", "/* 新增语句值 */[{0}]", (const char*)sql_value);
//
//					//新增数据
//					switch (conn->DatabaseKind)
//					{
//					case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
//					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
//					case DB_KIND_MSSQL:	        // MS SQL Server数据库
//					case DB_KIND_ORACLE:	        // Oracle 数据库
//					default:
//
//						sqlstr = sql_ins + sql_value;
//						break;
//					}
//
//					Log::Trace("", "", "/* 新增语句 */[{0}]", (const char*)sqlstr);
//
//					cmd_ins.SetCommandText(sqlstr);
//					cmd_ins.Parameters.Set("s_table_ename", s_table_ename);
//					cmd_ins.Parameters.Set("s.userid", s.userid);
//					cmd_ins.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
//					cmd_ins.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
//					cmd_ins.Parameters.Set("datetime", datetime);
//
//					//写表
//					cmd_ins.ExecuteNonQuery();
//#ifdef _SYS_MMS   /*是MMS系统时，发送电文到PES*/
//					Log::Trace("", "", "发送制造标准目的表[{0}]电文号[{1}]", s_table_ename, s_tc_no);
//					bcls_rec_q1.Tables[0].Rows[0]["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
//					bcls_rec_q1.Tables[0].Rows[0]["ST_NO"] = tqmts0x["ST_NO"];
//					bcls_rec_q1.Tables[0].Rows[0]["TABLE_NAME"] = s_table_ename;
//					bcls_rec_q1.Tables[0].Rows[0]["TC_NO"] = s_tc_no;
//					//doFlag = f_cm_0020q1_snd(&bcls_rec_q1, &bcls_ret_q1, conn);
//					if (doFlag != 0)
//					{
//						throw CApplicationException(-1, s.msg, log.Location);
//					}
//#endif
//				}//////增加制造标准结束
//			}
//			cmd_inq_tep02.Close();
//
//		}//////////结束
		
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
