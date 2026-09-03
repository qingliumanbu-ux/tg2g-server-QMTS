/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      张弘平
Version:     1.0
Date:        2014-06-17
Description: 炉次终判时,炉次代表成分的临时判定
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///炉次终判时,炉次代表成分的临时判定
/// <para>
/// 1.炉次终判时,炉次代表成分的临时判定
/// 
/// </para>
/// <para>数据库表：TQMTS29(实绩_炉次代表成分)		</para>
/// <para>主调用函数：前台QMTS21P画面的F6(判定)调用	</para>
/// <para>需调用函数：								</para>
/// </summary>
/// <param name="HEAT_NO">  熔炼号					</param>
/// <param name="ST_NO">  出钢记号					</param>
/// <returns>  </returns>
===========================================================</remark>*/


/*  外部函数申明  */
int f_qmts_jud(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);//判定

// service入口
BM2F_ENTERACE(qmts21p_jud)


int f_qmts21p_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
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
	CModel tep0002("TEP0002");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);

	try
	{
		//定义返回块的列名
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ELM_CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"ELM_OK");     //成分合否标志

		/* 对输入信息循环处理 */
		//for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ ) 
		{
			/* 取得单行传入信息 */

			tqmts29["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
			tqmts29["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();

			Log::Trace("","", "qmts21p_jud IN:---heat_no[{0}]",(const char*)tqmts29["HEAT_NO"].ToString());
			Log::Trace("", "", "qmts21p_jud IN:---st_no[{0}]",(const char*)tqmts29["ST_NO"].ToString());
			Log::Trace("", "", "qmts21p_jud IN:---st_no[{0}]", (const char*)tqmts29["ST_NO"].ToString());

			//校验传入参数
			if(tqmts29["HEAT_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("QM00S0004334")/*熔炼号不允许为空。*/);  
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts29["ST_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,"内部钢种不允许为空");  
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
					" WHERE HEAT_NO = @tqmts29.HEAT_NO "
					"   AND ST_NO = @tqmts29.ST_NO "
					" ORDER BY ELM_CODE ASC ";
				break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("tqmts29.HEAT_NO", tqmts29["HEAT_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("tqmts29.ST_NO", tqmts29["ST_NO"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts29);
			}
			cmd_inq.Close();

			//启动判定
			bcls_rec_f.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS29";
			bcls_rec_f.Tables[0].Rows[0]["HEAT_NO"] = tqmts29["HEAT_NO"];
			bcls_rec_f.Tables[0].Rows[0]["ST_NO"] = tqmts29["ST_NO"];
			bcls_rec_f.Tables[0].Rows[0]["PONO"] = tqmts29["PONO"];

			doFlag = f_qmts_jud(&bcls_rec_f,&bcls_ret_f,conn);

			if(doFlag != 0)
			{    
				Log::Trace("", "","f_qmts_jud() msg = [{0}]",s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
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
			Log::Trace("", "", "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tep0002);

				
				Log::Trace("", "","tep0002.code = [{0}]", (const char*)tep0002["CODE"].ToString());

				
				//2.查询每个元素对应的实绩数据
				switch(conn->DatabaseKind)
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
				cmd_inq_01.Parameters.Set("heat_no", tqmts29["HEAT_NO"].ToString().Trim());
				cmd_inq_01.Parameters.Set("elm_code", tep0002["CODE"].ToString().Trim());
				cmd_inq_01.ExecuteReader();
				if (cmd_inq_01.Read())
				{
					cmd_inq_01.Fetch(tqmts29);
					CDataRow& row = bcls_ret->Tables[0].Rows.Add();   //新增空行
					Log::Trace("", "","tqmts29.ELM_CODE= [{0}]",tqmts29["ELM_CODE"].ToString());
					Log::Trace("", "","tqmts29.ELM_OK = [{0}]",tqmts29["ELM_OK"].ToDecimal().ToDouble());
					row["ELM_CODE"] = tqmts29["ELM_CODE"];
					row["ELM_OK"]  = tqmts29["ELM_OK"].ToDecimal().ToString();
				}
				cmd_inq_01.Close();
			}
		}
		sprintf(s.msg,_RES("QM00S0004383")/*判定成功。*/);

		//临时弹出画面上的判定结果，不必写入TQMTS29表，所以回滚掉。
		CTransactionManager::Abort(0);
		CTransactionManager::Begin(0,0);
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
