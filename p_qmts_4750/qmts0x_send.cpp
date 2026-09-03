/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-11
Description: 下发工艺卡
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 下发工艺卡
/// <para>
/// 1.下发工艺卡和对应的成分标准信息
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡表);
//                  TQMTS02(工序成分标准表)                </para>
/// <para>主调用函数：前台QMTS0X画面的F11(下发)调用。       </para>
/// </summary>
/// <param name="ST_NO"> 出钢记号                          </param>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */
int f_qm002001_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);

// service入口
BM2F_ENTERACE(qmts0x_send)

int f_qmts0x_send(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{	
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	/*程序用变量*/
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	CString base_code = " ";
	/* 实体类定义 */
	CModel tqmts0x("TQMTS0X");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		base_code = bcls_rec->Tables[0].Rows[0]["base_code"].ToString().Trim();
		tqmts0x["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank();
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
		Log::Trace("", "", "qmts0x_send IN:---ST_NO = [{0}]tqmts0x.FACTORY_DIV = [{1}]", tqmts0x["ST_NO"].ToString(), tqmts0x["FACTORY_DIV"].ToString());
		
		//厂别区分可能会有多个不能写死，下发必须所有厂别一起下发
		//tqmts0x["FACTORY_DIV"] = "A";

		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT MIN(VALID_FLAG) "
				"   FROM TQMTS0X "
				"  WHERE ST_NO = @tqmts0x.ST_NO "
				"	 and FACTORY_DIV = @tqmts0x.FACTORY_DIV ";
			if (base_code.Trim() != "")
			{
				sqlstr += "    AND base_code = @base_code ";
			}
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tqmts0x.ST_NO", tqmts0x["ST_NO"].ToString());
		cmd_inq.Parameters.Set("base_code", base_code);
		cmd_inq.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x["FACTORY_DIV"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tqmts0x["VALID_FLAG"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
		Log::Trace("", "", "VALID_FLAG = [{0}]", (const char*)tqmts0x["VALID_FLAG"].ToString());
		if (tqmts0x["VALID_FLAG"].ToString() != "1")	
		{
			CFormattable arguments[] = {tqmts0x["ST_NO"].ToString()};
			CMessageFormat::Format(s.msg, _RES("QM00S0003887")/*该出钢记号[{0}]未审核，不能下发。*/, arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", "", "boforeSEND TC[002001]");
#ifdef _SYS_MMS
		Log::Trace("", "","SEND TC[002001]");
		doFlag = f_qm002001_snd(bcls_rec, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
#endif
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


