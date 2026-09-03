/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      张弘平
Version:     1.0
Date:        2014-06-16
Description: 炉次综判时弹出画面的查询-试样成分和标准查询
**************************************************/

//框架公用头文件
#include "stdafx.h"

//程序用头文件




/*<remark>=========================================================
/// <summary>
///炉次综判时弹出画面的查询-试样成分和标准查询
/// <para>
/// 1.炉次综判时弹出画面的查询-试样成分和标准查询
/// </para>
/// <para>数据库表：TQMTS29(实绩_工序成分)					</para>
/// <para>主调用函数：前台QMTS21的弹出画面QMTS21P的查询成分 </para>
/// <para>需调用函数：										</para>
/// </summary>
/// <returns>  </returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(qmts21p_inq_elm)

/*  外部函数申明  */
int f_qmts_jud(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//判定

int f_qmts21p_inq_elm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	CString s_heat_no = "";
	CString s_st_no = "";

	/* 实体类定义 */
	CModel tqmts02("TQMTS02");
	CModel tqmts29("TQMTS29");
	CModel tep0002("TEP0002");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);
	CDbCommand cmd_ep(conn);   //代码查询用

	EIClass bcls_rec_f;   //计算组合元素、判定用
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f.Tables[0].Rows.Add();

	try
	{
		//定义返回块的列名
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ELM_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ELM_NAME");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"MAIN_AIM");   //元素主试目标值
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"MAIN_MIN");   //元素主试最小值
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"MAIN_MAX");   //元素主试最大值
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ELM_VALUE");  //元素实际值
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ELM_OK");     //成分合否标志
		//bcls_ret->Tables[0].Columns.Add(DT_STRING,"ELM_VALID");  //成分有效标志, 前台计算用,0-无效

		//-----------------------------------------
		//取得单行传入信息
		s_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		s_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		
		Log::Trace("", "","qmts21p_inq_elm IN:---heat_no = [{0}]",(const char*)s_heat_no);
		Log::Trace("", "","qmts21p_inq_elm IN:---st_no = [{0}]",s_st_no);
		
		//校验传入参数
		if(s_heat_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg,_RES("QM00S0004334")/*熔炼号不允许为空。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TQMTS29 "
				" WHERE HEAT_NO = @s_heat_no "
				"   AND ST_NO = @s_st_no "
				" ORDER BY ELM_CODE ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("s_heat_no", s_heat_no);
		cmd_inq.Parameters.Set("s_st_no", s_st_no);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts29);
		}
		cmd_inq.Close();

		Log::Trace("", "", "qmts21p_inq_elm IN:---tqmts29.PONO= [{0}]", tqmts29["PONO"].ToString());

		if (tqmts29["PONO"].ToString().TrimOrBlank() != " ")
		{
			//启动判定
			bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS29";
			bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = s_heat_no;
			bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = s_st_no;
			bcls_rec_f.Tables[0].Rows[0]["PONO"] = tqmts29["PONO"];

			doFlag = f_qmts_jud(&bcls_rec_f, &bcls_ret_f, conn);

			if (doFlag != 0)
			{
				Log::Trace("", "", "f_qmts_jud() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}

		//----------------------------------------------------------
		//查询指定试样号下的元素标准和实绩
		//1.按代码定义元素顺序取值
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = "SELECT * "
				"  FROM TEP0002 "
				" WHERE CODE_CLASS = 'QMYS' "
				"   AND trim(CODE_DESC_2_CONTENT) is not null "
				" ORDER BY CODE_DESC_2_CONTENT ASC ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_inq.Fetch(tep0002);

			 
			Log::Trace("", "","tep0002.code = [{0}]", (const char*)tep0002["CODE"].ToString());
			Log::Trace("", "", "s_heat_no = [{0}]", (const char*)s_heat_no);

			//2.查询每个元素对应的实绩数据
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TQMTS29 "
					" WHERE HEAT_NO = @heat_no "
					"   AND ELM_CODE = @elm_code ";
				break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("heat_no", s_heat_no);
			cmd_inq_01.Parameters.Set("elm_code", tep0002["CODE"].ToString());
			cmd_inq_01.ExecuteReader();
			if (cmd_inq_01.Read())
			{
				cmd_inq_01.Fetch(tqmts29);
				Log::Trace("", "", "tqmts29.ELM_VALUE = [{0}]", tqmts29["ELM_VALUE"].ToDecimal().ToDouble());
				Log::Trace("", "", "tqmts29.ELM_OK = [{0}]", tqmts29["ELM_OK"].ToDecimal().ToDouble());
			}
			else
			{
				tqmts29["ELM_VALUE"] = -1;    // 没有元素实绩的
				tqmts29["ELM_OK"] = 0;
				Log::Trace("", "", "tqmts29.ELM_VALUE ********* = [{0}]", tqmts29["ELM_VALUE"].ToDecimal().ToDouble());
				Log::Trace("", "", "tqmts29.ELM_OK ********* = [{0}]", tqmts29["ELM_OK"].ToDecimal().ToDouble());
			}
			cmd_inq_01.Close();

			//3.读取标准
			switch(conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * "
					"  FROM TQMTS02 "
					" WHERE ST_NO = @st_no "
					"   AND WHOLE_BACKLOG_CODE = @whole_backlog_code "
					"   AND ELM_CODE = @code ";
				break;
			}
			cmd_inq_01.SetCommandText(sqlstr);
			cmd_inq_01.Parameters.Set("st_no", s_st_no);
			cmd_inq_01.Parameters.Set("whole_backlog_code", "G"); //写死显示工艺卡上的标准
			cmd_inq_01.Parameters.Set("code", tep0002["CODE"].ToString());
			cmd_inq_01.ExecuteReader();
			if (cmd_inq_01.Read())
			{
				cmd_inq_01.Fetch(tqmts02);
				Log::Trace("", "", "tqmts02.ELM_CODE = [{0}]", tqmts02["ELM_CODE"].ToString());
				Log::Trace("", "", "tqmts02.MAIN_AIM = [{0}]", tqmts02["MAIN_AIM"].ToDecimal());
				Log::Trace("", "", "tqmts02.factory_div = [{0}]", tqmts02["FACTORY_DIV"]);
			}
			else
			{
				tqmts02["ELM_CODE"] = tep0002["CODE"];
				tqmts02["ELM_NAME"] = tep0002["CODE_DESC_1_CONTENT"];
				//tqmts02["MAIN_MIN"] = 0;
				//tqmts02["MAIN_MAX"] = 99.999;
				tqmts02["MAIN_AIM"] = -1;
				//tqmts02["SPE_MIN"]  = 0;
				//tqmts02["SPE_MAX"]  = 99.999;
				Log::Trace("", "", "tqmts02.MAIN_AIM ********* = [{0}]", tqmts02["MAIN_AIM"].ToDecimal());
			}
			cmd_inq_01.Close();

			CDataRow& row = bcls_ret->Tables[0].Rows.Add();   //新增空行
			row["ELM_CODE"] = tqmts02["ELM_CODE"];
			row["ELM_NAME"] = tqmts02["ELM_NAME"];

			row["ELM_VALUE"] = tqmts29["ELM_VALUE"];
			row["ELM_OK"] = tqmts29["ELM_OK"];

			Log::Trace("", "", "tqmts02.ELM_CODE = [{0}]", tqmts02["ELM_CODE"].ToString());
			Log::Trace("", "", "tqmts02.ELM_NAME = [{0}]", tqmts02["ELM_NAME"].ToString());
			Log::Trace("", "", "tqmts29.ELM_VALUE = [{0}]", tqmts29["ELM_VALUE"].ToDecimal());
			Log::Trace("", "", "tqmts29.ELM_OK = [{0}]", tqmts29["ELM_OK"].ToDecimal());

			if (tqmts02["MAIN_AIM"].ToDecimal() == -1) //tqmts02表中没有该元素标准。 表示不做判定
			{
				row["MAIN_AIM"] = "-1";
				row["MAIN_MAX"] = "-1";
				row["MAIN_MIN"] = "-1";
				row["ELM_OK"] = "9"; /*不需判*/
				Log::Trace("", "", "tqmts29.ELM_OK****** = [9]不需判");
			}
			else
			{
				row["MAIN_AIM"] = tqmts02["MAIN_AIM"];
				row["MAIN_MAX"] = tqmts02["MAIN_MAX"];
				row["MAIN_MIN"] = tqmts02["MAIN_MIN"];
				Log::Trace("", "", "tqmts02.MAIN_AIM ****** = [{0}]", tqmts02["MAIN_AIM"].ToDecimal());
				Log::Trace("", "", "tqmts02.MAIN_MAX ****** = [{0}]", tqmts02["MAIN_MAX"].ToDecimal());
				Log::Trace("", "", "tqmts02.MAIN_MIN ****** = [{0}]", tqmts02["MAIN_MIN"].ToDecimal());
			}
			Log::Trace("", "", "tep0002.code = [{0}]", tep0002["CODE"].ToString());
		}
		cmd_inq.Close();
		CTransactionManager::Abort(0);
		CTransactionManager::Begin(0, 0);
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

