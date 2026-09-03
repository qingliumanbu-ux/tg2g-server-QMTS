/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-16
Description: 炉次代表成分判定
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炉次代表成分判定
/// <para>
/// 1.炉次代表成分判定
/// 
/// </para>
/// <para>数据库表：TQMTS29(实绩_炉次代表成分)		</para>
/// <para>主调用函数：前台QMTS29画面的F6(判定)调用	</para>
/// <para>需调用函数：								</para>
/// </summary>
/// <param name="HEAT_NO">  熔炼号					</param>
/// <param name="ST_NO">  出钢记号					</param>
/// <returns>  </returns>
===========================================================</remark>*/


/*  外部函数申明  */
int f_qmts_jud(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);//判定

// service入口
BM2F_ENTERACE(qmts29_chk)


int f_qmts29_chk(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";

	EIClass bcls_rec_f;   //计算组合元素、判定用
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"ST_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING, "PONO");
	bcls_rec_f.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts29("TQMTS29");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 对输入信息循环处理 */
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ ) 
		{
			/* 取得单行传入信息 */
			tqmts29.Reset();
			tqmts29.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","tqmts29_ins IN:---heat_no[{0}]",(const char*)tqmts29["HEAT_NO"].ToString());

			//校验传入参数
			if(tqmts29["HEAT_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("QM00S0004334")/*熔炼号不允许为空。*/);  
				throw CApplicationException(-1, s.msg, log.Location);
			}

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT ST_NO "
							 "  FROM TQMTS29 "
							 " WHERE HEAT_NO = @heat_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts29["HEAT_NO"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				tqmts29["ST_NO"] = cmd_inq.GetString(1);
			}
			else
			{
				sprintf(s.msg, "熔炼号[%s]的代表成分不存在，请先新增！",(const char*)tqmts29["HEAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();
			Log::Trace("", "","ST_NO[{0}]",(const char*)tqmts29["ST_NO"].ToString());

			//启动判定
			bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS29";
			bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = tqmts29["HEAT_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = tqmts29["ST_NO"];
			bcls_rec_f.Tables[0].Rows[0]["PONO"] = tqmts29["PONO"];
			doFlag = f_qmts_jud(&bcls_rec_f, &bcls_ret_f, conn);
			if(doFlag != 0)
			{    
				Log::Trace("", "","f_qmts_jud() msg = [{0}]",s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		sprintf(s.msg,_RES("QM00S0004383")/*判定成功。*/);
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
