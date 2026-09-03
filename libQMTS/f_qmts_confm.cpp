/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   chenwenqiong
Version:    1.0
Date:     2014-05-16 11:23:56
Description: MES炉次确定写质量函数
**************************************************/

#include "stdafx.h"

#include "tqmts29.h"
#include "tmmsm01.h"
#include "tqmtqq0.h"
#include "tqmtqb0.h"


/* ***** 外部函数申明 ***** */
//调用计划函数取炉次确定电文计划部分数据

BM2_FUNCTION_IMPORT
 int f_pssm32_ht_inq(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);

/*<remark>=========================================================
/// <summary>
///获取炉次确定的成分写Q0表和B0表
/// <para>
/// MES系统炉次确定时，计划模块调用，读TQMTS29表和TQMTS24表，写入TQMTSQ0实绩表_原料化学成份表和TQMTSB0炉次信息表。
/// </para>
/// <para>数据库表：TQMTS29/24/Q0/B0
/// <para>主调用函数：该接口函数在炉次确定时由计划模块调用
/// <para>需调用函数： 
/// </summary>
/// <param name="ST_NO">PONO,HEAT_NO,             </param>
/// <returns>  </returns>
===========================================================</remark>*/
BM2_FUNCTION_EXPORT
 int f_qmts_confm(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//变量声明
	CString sqlstr("");
	int doFlag = 0;
	int i = 0;

	EIClass bcls_rec_ps;//调用计划函数取炉次确定电文计划部分数据
	EIClass bcls_ret_ps;

	/* 实体类定义 */
	CTQMTS29 tqmts29(conn);
	CTMMSM01 tmmsm01(conn);
	CTQMTQQ0 tqmtqq0(conn);
	CTQMTQB0 tqmtqb0(conn);
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq1(conn);

	try
	{  
		/* ***** 定义输入块的列,为调用函数准备 ***** */
		bcls_rec_ps.Clear();
		bcls_rec_ps.Tables.Add();
		bcls_rec_ps.Tables[0].Columns.Add(DT_STRING,"PONO");	//制造命令号
		bcls_rec_ps.Tables[0].Columns.Add(DT_STRING,"HEAT_NO");	//熔炼号

		/* ***** 获取输入参数 ***** */
		tqmtqb0.PONO = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		tqmtqb0.HEAT_NO = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();

		/* ***** 打印输入参数 ***** */
		Log::Trace("", __FUNCTION__, "tqmtqb0.PONO[{0}]", (const char*)tqmtqb0.PONO);
		Log::Trace("", __FUNCTION__, "tqmtqb0.HEAT_NO[{0}]", (const char*)tqmtqb0.HEAT_NO);

		/* ***** 程序处理 ***** */
		/* ***** 1、调用计划函数，取炉次确定计划部分数据 ***** */
		Log::Trace("", __FUNCTION__, "* ***** 1、调用计划函数，取炉次确定计划部分数据 ***** *");
		CDataRow& row1 = bcls_rec_ps.Tables[0].Rows.Add();
		row1["PONO"] = tqmtqb0.PONO;
		row1["HEAT_NO"] = tqmtqb0.HEAT_NO;
		doFlag = f_pssm32_ht_inq(&bcls_rec_ps, &bcls_ret_ps,conn);
		if(doFlag != 0)
		{
			Log::Trace("", __FUNCTION__, "f_pssm32_ht_inq() msg = [{0}]",s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		else
		{     
			tqmtqb0.ST_NO = bcls_ret_ps.Tables[0].Rows[0]["DECI_ST_NO"].ToString().Trim();
		}
		Log::Trace("", __FUNCTION__, "DECI_ST_NO[{0}]", (const char*)tqmtqb0.ST_NO);
		tqmtqb0.TrimOrBlank();


		/* ***** 2、获取质量部分炉次信息数据 ***** */
		//写入炉次信息表
		Log::Trace("", "","3.1 写入炉次信息表");
		
		//add by wyl 110707 为了可以重复接收，先删再写
		tqmtqb0.Delete("PONO");

		tqmtqb0.VENDOR_CODE = "system";
		tqmtqb0.REC_CREATOR = s.userid;
		tqmtqb0.REC_CREATE_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tqmtqb0.Insert();


		/* ***** 3、获取质量部分炉次化学成分信息 ***** */
		//取代表成分
		//定义数据库操作命令对象comm_inq执行sql语句，sql字符串用""包括，可以分行，但每行前后务必留出一个空格。
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = " SELECT elm_code,elm_name,elm_pos,elm_unit,elm_value "
				" FROM TQMTS29 "
				" WHERE pono = @pono ";
			break;
		}
		cmd_inq1.SetCommandText(sqlstr);		
		//压入绑定参数，""包括的第一个参数的名称与SQL语句中@后对应的变量完全一致，大小写敏感
		cmd_inq1.Parameters.Set("pono", tqmtqb0.PONO);
		cmd_inq1.ExecuteReader();
		while (cmd_inq1.Read())
		{
			//写入实绩表_原料化学成份
			Log::Trace("", "","3.3 写入实绩表_原料化学成份");
			tqmtqq0.Reset();
			tqmtqq0.PONO = tqmtqb0.PONO;
			tqmtqq0.HEAT_NO = tqmtqb0.HEAT_NO;
			tqmtqq0.VENDOR_CODE = "system";
			tqmtqq0.ST_NO = tqmtqb0.ST_NO;

			tqmtqq0.ELM_CODE = cmd_inq1.GetString(1);
			tqmtqq0.ELM_NAME = cmd_inq1.GetString(2);
			tqmtqq0.ELM_POS = cmd_inq1.GetDecimal(3);
			tqmtqq0.ELM_UNIT = cmd_inq1.GetString(4);
			tqmtqq0.ELM_ACT = cmd_inq1.GetDecimal(5);
			tqmtqq0.REC_CREATOR = tqmtqb0.REC_CREATOR;
			tqmtqq0.REC_CREATE_TIME = tqmtqb0.REC_CREATE_TIME;	

			Log::Trace("", "", "tqmtqq0.ELM_CODE[{0}]", (const char*)tqmtqq0.ELM_CODE);

			if(tqmtqq0.ELM_CODE.Trim() == ""||tqmtqq0.ELM_CODE.Trim() == " ")
			{

				break;
			}
			tqmtqq0.Insert();
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


	//在函数退出前，统一Close()操作
	cmd_inq1.Close();

	return doFlag;
}
