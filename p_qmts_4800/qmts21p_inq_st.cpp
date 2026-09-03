/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      张弘平
Version:     1.0
Date:        2014-06-16
Description: 炉次终判时的弹出画面查询可选的内部钢种
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
///炉次终判时的弹出画面查询可选的内部钢种
/// <para>
/// 1.炉次终判时的弹出画面查询可选的内部钢种
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡)		</para>
/// <para>主调用函数：前台QMTS21的弹出画面QMTS21P的查询内部钢种号</para>
/// <para>需调用函数：										</para>
/// </summary>
/// <returns>  </returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts21p_inq_st)


int f_qmts21p_inq_st(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CDecimal v_main_max = 0, v_main_min = 0, v_spe_max = 0, v_spe_min = 0;
	CString remark = "", remark1 = "", remark2 = "", remark3 = "", remark4 = "";

	CString sqlstr = "";
	CString HEAT_NO = "", ELM_NAME="";

	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_ts02(conn);

	try
	{
		//定义返回块的列名
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ST_NO");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"VALID_FLAG");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"LABEL1");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "FACTORY_DIV");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "REMARK");

		/* 获得输入参数 */
		tqmts0x["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		tqmts0x["LABEL1"] = bcls_rec->Tables[0].Rows[0]["LABEL1"].ToString().Trim();

		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO")) HEAT_NO = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();

		Log::Trace("", "","qmts21p_inq_st IN:---ST_NO = [{0}]  FACTORY_DIV= [{1}] LABEL1= [{2}]",tqmts0x["ST_NO"].ToString(),tqmts0x["FACTORY_DIV"].ToString(),tqmts0x["LABEL1"].ToString());
		Log::Trace("", "","qmts21p_inq_st IN:---HEAT_NO = [{0}] ", HEAT_NO);

		//2023.2.2 取判钢队列表 出钢记号，并判钢
		if (HEAT_NO != "")
		{
			//1.取出钢中队列
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT b.ST_NO,b.VALID_FLAG,b.LABEL1,b.FACTORY_DIV,b.REMARK FROM TQMTS6C a, TQMTS0X b WHERE 1=1 and a.ST_NO_JJ=b.st_no AND b.VALID_FLAG ='1' ";
				sqlstr += " AND st_no In (select st_no from TQMTS23 where heat_no='" + HEAT_NO + "')";
			//	sqlstr += " ORDER BY SEQ_NO ASC";
				break;
			}
			Log::Trace("", "", "qmts21p_inq_st 1-sqlstr[{0}]", sqlstr);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				i++;
				cmd_inq.Fetch(tqmts0x);

				CDataRow& row = bcls_ret->Tables[0].Rows.Add();   //新增空行
				row["ST_NO"] = tqmts0x["ST_NO"];
				row["VALID_FLAG"] = tqmts0x["VALID_FLAG"];
				row["LABEL1"] = tqmts0x["LABEL1"]; //适用牌号
				row["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];

				//2.按钢种 取标准值  返回超标 元素
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					//sqlstr = "SELECT * FROM TQMTS02 WHERE 1=1 and SMELT_CHEMI_FLAG='3' AND WHOLE_BACKLOG_CODE ='G' ";
					//sqlstr += " AND st_no = '" + tqmts0x["ST_NO"] + "'";	
					sqlstr = "SELECT  (b.ELM_VALUE -a.MAIN_MAX ) v1,(b.ELM_VALUE -a.MAIN_MIN ) v2,(b.ELM_VALUE -a.SPE_MAX ) v3,(b.ELM_VALUE -a.SPE_MIN ) v4,b.ELM_NAME,b.ELM_VALUE,a.* ";
					sqlstr += "	FROM TQMTS02 a, tqmts29 b WHERE a.st_no = 'GV5666B1' and a.SMELT_CHEMI_FLAG = '3' AND a.WHOLE_BACKLOG_CODE = 'G'";
					sqlstr += "	and a.elm_code = b.elm_code and b.pono = '1301465' ";
					break;
				}
				Log::Trace("", "", "qmts21p_inq_st 2-sqlstr[{0}]", sqlstr);
				remark = "";
				remark1 = "";
				remark2 = "";
				remark3 = "";
				remark4 = "";
				cmd_ts02.SetCommandText(sqlstr);
				cmd_ts02.ExecuteReader();
				while (cmd_ts02.Read())
				{
					v_main_max = cmd_ts02.GetDecimal(1);
					v_main_min = cmd_ts02.GetDecimal(2);
					v_spe_max = cmd_ts02.GetDecimal(3);
					v_spe_min = cmd_ts02.GetDecimal(4);
					ELM_NAME = cmd_ts02.GetString(5);
					//Log::Trace("", "", "ELM_NAME[{0}]", ELM_NAME);
					if (v_main_max > 0)
					{						
						remark1 = ELM_NAME + "高(内控)";
						if (remark != "") remark += ",";
						remark += remark1;

					}
					if (v_main_min < 0)
					{
						remark2 = ELM_NAME + "低(内控)";
						if (remark != "") remark += ",";
						remark += remark2;
					}
					if (v_spe_max > 0)
					{
						remark3 = ELM_NAME + "高";
						if (remark != "") remark += ",";
						remark += remark3;
					}
					if (v_spe_min < 0)
					{
						remark4 = ELM_NAME + "低";
						if (remark != "") remark += ",";
						remark += remark4;
					}

					
				}
				cmd_ts02.Close();

				row["REMARK"] = remark;
				//

			}
			cmd_inq.Close();
		}
		//

		if (i <= 0)
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * FROM TQMTS0X WHERE 1=1  ";
				if (tqmts0x["ST_NO"].ToString().Trim() != "")	sqlstr += " AND ST_NO like '%" + tqmts0x["ST_NO"].ToString().Trim() + "%' ";
				if (tqmts0x["LABEL1"].ToString().Trim() != "")		sqlstr += " AND LABEL1 like '%" + tqmts0x["LABEL1"].ToString().Trim() + "%' ";
				if (tqmts0x["FACTORY_DIV"].ToString().Trim() != "")		sqlstr += " AND FACTORY_DIV like '%" + tqmts0x["FACTORY_DIV"].ToString().Trim() + "%' ";

				sqlstr += " AND VALID_FLAG in ('1') ";  //显示已经生效的
				sqlstr += " ORDER BY ST_NO ASC";
				break;
			}
			Log::Trace("", "", "qmts21p_inq_st 0--sqlstr[{0}]", sqlstr);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts0x);

				CDataRow& row = bcls_ret->Tables[0].Rows.Add();   //新增空行
				row["ST_NO"] = tqmts0x["ST_NO"];
				row["VALID_FLAG"] = tqmts0x["VALID_FLAG"];
				row["LABEL1"] = tqmts0x["LABEL1"]; //适用牌号
				row["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];

			}
			cmd_inq.Close();
		}

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
