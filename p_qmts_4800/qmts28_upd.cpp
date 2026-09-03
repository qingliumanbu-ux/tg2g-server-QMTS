/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-20
Description: 修改宏观检化验委托信息
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///修改宏观检化验委托信息
/// <para>
/// 1.修改宏观检化验委托信息
/// 
/// </para>
/// <para>数据库表：TQMTS28(宏观检化验委托信息表)		   </para>
/// <para>主调用函数：前台QMTS28画面的F4(修改)调用		   </para>
/// <para>需调用函数：							           </para>
/// </summary>
/// <param name="SLAB_NO">  材料号				           </param>
/// <returns>  </returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts28_upd)


int f_qmts28_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";
	
	/* 实体类定义 */
	CModel tqmts2a("TQMTS2A");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//宏观检化验委托信息成分标准
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{
			//取得单行传入信息
			tqmts2a.Reset();
			tqmts2a.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","qmts28_upd IN:---SLAB_NO = [{0}]",(const char*)tqmts2a["SLAB_NO"].ToString());
			Log::Trace("", "","qmts28_upd IN:---SAMPLE_POS_CODE = [{0}]",(const char*)tqmts2a["SAMPLE_POS_CODE"].ToString());

			//校验传入参数
			if(tqmts2a["SLAB_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("GCRSS0000035")/*材料号不能为空*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts2a["SAMPLE_POS_CODE"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("QM00S0004341")/*取样位置不可为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT SEND_NUM "
							 "  FROM TQMTS2A "
							 " WHERE SLAB_NO = @mat_no "
							 "   AND SAMPLE_POS_CODE = @sample_pos_code";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", tqmts2a["SLAB_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("sample_pos_code", tqmts2a["SAMPLE_POS_CODE"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts2a["SEND_NUM"] = cmd_inq.GetDecimal(1);
			}
			cmd_inq.Close();
			if(tqmts2a["SEND_NUM"].ToDecimal() > 0)
			{
				sprintf(s.msg,"材料号[%s]取样位置[%s]的宏观检化验委托已发送，不能修改",(const char*)tqmts2a["SLAB_NO"].ToString(),(const char*)tqmts2a["SAMPLE_POS_CODE"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT VERSION,HEAT_NO,PONO,ST_NO "
							 "  FROM TQMTS2A "
							 " WHERE SLAB_NO = @slab_no "
							 "   AND SAMPLE_POS_CODE = @sample_pos_code ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("slab_no", tqmts2a["SLAB_NO"].ToString().Trim());
			cmd_inq.Parameters.Set("sample_pos_code", tqmts2a["SAMPLE_POS_CODE"].ToString().Trim());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts2a["VERSION"] = cmd_inq.GetDecimal(1);
				tqmts2a["HEAT_NO"] = cmd_inq.GetString(2);
				tqmts2a["PONO"] = cmd_inq.GetString(3);
				tqmts2a["ST_NO"] = cmd_inq.GetString(4);

				tqmts2a.Delete("SLAB_NO, SAMPLE_POS_CODE"); //条件字段项
			}
			else
			{
				sprintf(s.msg,"材料号[%s]取样位置[%s]的宏观检化验委托不存在，请先生成委托",(const char*)tqmts2a["SLAB_NO"].ToString(),(const char*)tqmts2a["SAMPLE_POS_CODE"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
			
			/******************** 赋初值 *************************/
			tqmts2a["REC_CREATOR"] = s.userid;
			tqmts2a["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts2a["REC_REVISOR"] = " ";
			tqmts2a["REC_REVISE_TIME"] = " ";
			tqmts2a["ARCHIVE_FLAG"] = " ";
			tqmts2a["DU_FLAG"] = " ";
			tqmts2a["DU_MAKER"] = " ";
			tqmts2a["DU_TIME"] = " ";
			tqmts2a["VERSION"] = tqmts2a["VERSION"].ToDecimal() + 1;

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT CODE_DESC_1_CONTENT "
							 "  FROM TEP0002 "
							 " WHERE CODE_CLASS = 'QM45' "
							 "   AND CODE = @sample_pos_code ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("sample_pos_code", tqmts2a["SAMPLE_POS_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts2a["SAMPLE_POS"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			Log::Trace("", "","tqmts2a.SAMPLE_POS         	  = [{0}]", (const char*)tqmts2a["SAMPLE_POS"].ToString());

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = "SELECT CODE_DESC_1_CONTENT "
							 "  FROM TEP0002 "
							 " WHERE CODE_CLASS = 'QMZE' "
							 "   AND CODE = @sample_mode_code ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("sample_mode_code", tqmts2a["SAMPLE_MODE_CODE"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts2a["SAMPLE_MODE"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			Log::Trace("", "","tqmts2a.SAMPLE_MODE         	  = [{0}]", (const char*)tqmts2a["SAMPLE_MODE"].ToString());

			tqmts2a.TrimOrBlank();
			tqmts2a.Insert();
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
