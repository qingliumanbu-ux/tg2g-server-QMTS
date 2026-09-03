/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 修改制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中




/*<remark>=========================================================
/// <summary>
/// 修改制造标准
/// <para>
/// 获取输入参数：st_no(出钢记号)
/// </para>
/// <para>数据库表：TQMTS02(工序成分标准表)
/// 前台各个QMTS0x画面的F3(修改)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各个制造标准表    </param>
===========================================================</remark>*/ 

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmts_updchemi)


int f_qmts_updchemi(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;

	CString s_st_no = " ";
	CString s_whole_backlog_code = " ";
	CString base_code = "";
	CDecimal i_count = 0;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	/* 实体类定义 */
	CModel tqmts02("TQMTS02");
	CModel tep0002("TEP0002");
	CModel tqmts0x("TQMTS0X");
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		s_st_no					= bcls_rec->Tables[1].Rows[0]["st_no"].ToString().Trim();
		s_whole_backlog_code	= bcls_rec->Tables[1].Rows[0]["whole_backlog_code"].ToString().Trim();
		base_code               = bcls_rec->Tables[1].Rows[0]["base_code"].ToString().Trim();
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[1].Rows[0]["factory_div"].ToString().TrimOrBlank();  ////update by yiling 20170224

		Log::Trace("", "", "qmts_updchemi IN: s_st_no[{0}]",s_st_no);
		Log::Trace("", "", "qmts_updchemi IN: s_whole_backlog_code[{0}]tqmts0x.FACTORY_DIV[{1}]", s_whole_backlog_code, tqmts0x["FACTORY_DIV"].ToString());

		if (s_st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0006260")/*内部钢种不能为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		tqmts0x["ST_NO"] = s_st_no;
		tqmts0x["BASE_CODE"] = base_code;
		tqmts0x.Query("ST_NO,FACTORY_DIV,BASE_CODE");
		//调用检查出钢记号是否在计划中使用的函数
		//bcls_rec_s.Tables[0].Rows.Add();
		//bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = "A";
		//bcls_rec_s.Tables[1].Rows.Add();
		//bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = s_st_no;
		//doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);
		//if(doFlag != 0)
		//{    
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		//制造标准成分标准
		for (i = 0; i <bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//取得单行传入信息
			tqmts02.Reset();
			tqmts02.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (tqmts02["ELM_CODE"].ToString().TrimOrBlank() == " ")
			{
				sprintf(s.msg, _RES("QM00S0004168")/*元素代码不能为空。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (tqmts02["SMELT_CHEMI_FLAG"].ToString().TrimOrBlank() == " ")
			{
				sprintf(s.msg, "取样指示不能为空");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			tqmts02["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			tqmts02["ST_NO"] = s_st_no;
			tqmts02["WHOLE_BACKLOG_CODE"] = s_whole_backlog_code;
			tqmts02["BASE_CODE"] = base_code;

			if (tqmts02["MAIN_MIN"].ToDecimal() > tqmts02["MAIN_MAX"].ToDecimal() || tqmts02["MAIN_AIM"].ToDecimal() > tqmts02["MAIN_MAX"].ToDecimal() || tqmts02["MAIN_MIN"].ToDecimal() > tqmts02["MAIN_AIM"].ToDecimal())
			{
				CFormattable arguments[] = { (const char*)tqmts02["ST_NO"].ToString(), (const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString(), (const char*)tqmts02["ELM_NAME"].ToString() };
				CMessageFormat::Format(s.msg, _RES("QM00S0004178")/*出钢记号[{0}]工序[{1}]对应的[{2}]元素主试成分数据倒置。*/, arguments, 3);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (tqmts02["SPE_MIN"].ToDecimal() > tqmts02["SPE_MAX"].ToDecimal())
			{
				CFormattable arguments[] = { (const char*)tqmts02["ST_NO"].ToString(), (const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString(), (const char*)tqmts02["ELM_NAME"].ToString() };
				CMessageFormat::Format(s.msg, _RES("QM00S0004001")/*出钢记号[{0}]工序[{1}]对应的[{2}]元素特采成分数据倒置。*/, arguments, 3);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//根据成分标准信息是否不存在，确定是新增，还是修改。
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT COUNT(1) "
					"  FROM TQMTS02 "
					" WHERE FACTORY_DIV  = @factory_div "
					"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
					"   AND ST_NO = @st_no "
					"   AND BASE_CODE =@base_code"
					"   AND ELM_CODE = @elm_code ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
			cmd_inq.Parameters.Set("factory_div", tqmts02["FACTORY_DIV"].ToString());
			cmd_inq.Parameters.Set("base_code", base_code);
			cmd_inq.Parameters.Set("whole_backlog_code", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
			cmd_inq.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString());
			i_count = cmd_inq.ExecuteScalar();	//ExecuteScalar只返回查询结果集中的第一行的第一列，忽略额外的列或行，适用于单纯计算COUNT,SUM,MAX等的sql语句。
			cmd_inq.Close();

			if (i_count <= 0)	//成分信息不存在，先新增，再修改。
			{
				/******************** 赋初值 *************************/
				tqmts02["REC_CREATOR"] = s.userid;
				tqmts02["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts02["REC_REVISOR"] = " ";
				tqmts02["REC_REVISE_TIME"] = " ";
				tqmts02["ARCHIVE_FLAG"] = " ";
				tqmts02["VERSION"] = 1;
				tqmts02["DU_FLAG"] = " ";
				tqmts02["DU_MAKER"] = " ";
				tqmts02["DU_TIME"] = " ";

				//获取工序顺序号
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT WHOLE_BACKLOG_SEQ "
						"  FROM TQMTS01 "
						" WHERE FACTORY_DIV  = @factory_div "
						"   AND BASE_CODE =@base_code"
						"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
						"   AND ST_NO = @st_no ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				cmd_inq.Parameters.Set("factory_div", tqmts02["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("whole_backlog_code", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts02["WHOLE_BACKLOG_SEQ"] = cmd_inq.GetInt32(1);
				}
				cmd_inq.Close();

				//获取元素顺序、元素单位
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
						"   AND CODE = @elm_code ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cmd_inq.Fetch(tep0002);
					tqmts02["ELM_POS"] = CDecimal::Parse(tep0002["CODE_DESC_2_CONTENT"].ToString());//得到元素顺序
					tqmts02["ELM_UNIT"] = "%";
				}
				cmd_inq.Close();

				tqmts02.TrimOrBlank();
				tqmts02.Insert();
			}
			else       //成分信息存在，直接修改。
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT VERSION "
						"  FROM TQMTS02 "
						" WHERE FACTORY_DIV  = @factory_div "
						"   AND ST_NO = @st_no "
						"   AND BASE_CODE =@base_code"
						"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
						"   AND ELM_CODE = @elm_code ";
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("factory_div", tqmts02["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("st_no", tqmts02["ST_NO"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				cmd_inq.Parameters.Set("whole_backlog_code", tqmts02["WHOLE_BACKLOG_CODE"].ToString());
				cmd_inq.Parameters.Set("elm_code", tqmts02["ELM_CODE"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts02["VERSION"] = cmd_inq.GetInt32(1);
				}
				cmd_inq.Close();

				tqmts02["REC_REVISOR"] = s.userid;
				tqmts02["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tqmts02["VERSION"] = tqmts02["VERSION"].ToDecimal() + 1;

				//修改成分标准内容
				tqmts02.Update("REC_REVISOR, REC_REVISE_TIME, VERSION, MAIN_MIN, MAIN_MAX, MAIN_AIM, SPE_MIN, SPE_MAX, SMELT_CHEMI_FLAG, ROUND_CODE, ELM_ACCU");
			}
		}

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
