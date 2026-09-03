/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-22
Description: 工艺卡电文接收
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
#include "epex.h"

//程序用头文件，请包含在""中
#include "tqmts0x.h"
#include "tqmts02.h"
#include "tep0002.h"

/*<remark>=========================================================
/// <summary>
/// 工艺卡电文接收
/// 电文号002001
/// <para>
/// 获取输入参数：出钢记号，处理标记 ；
/// </para>
/// <para>数据库表：TQMTS0X(工艺卡表);
//                  TQMTS02(工序成分标准表)
                    TQMTS01(工序制造标准表)
					TQMTS03(制造标准_铁水预处理)
					TQMTS04(制造标准_转炉)
					TQMTS05(制造标准_VD/VOD)
					TQMTS06(制造标准_RH)
					TQMTS07(制造标准_LF)
					TQMTS08(制造标准_连铸)
					TQMTS0A(制造标准_电炉)   </para>
/// </summary>
/// <param name="ST_NO">出钢记号    </param>
/// <param name="DIFF">处理标记    </param>
===========================================================</remark>*/  

// service入口
BM2F_ENTERACE_TELE(cm_002001_rcv)


int f_cm_002001_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{	
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	CString s_table_name = "";
	CString sqlstr_tep02("");
	int doFlag = 0;
	int i = 0;
	int v_diff = 0;

	/* 实体类定义 */
	CTQMTS0X tqmts0x(conn);
	CTQMTS02 tqmts02(conn);
	CTEP0002 tep0002(conn);
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_del(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_tep02(conn);

	try
	{
		/* ***** 获取输入参数 ***** */
		tqmts0x.Reset();
		tqmts0x.ST_NO = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
		tqmts0x.FACTORY_DIV = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().TrimOrBlank();
		v_diff = (int)bcls_rec->Tables[0].Rows[0]["diff"];
		
		/* ***** 打印输入参数 ***** */
		Log::Trace("", "","cm_002001_rcv IN: diff = [{0}]",v_diff);
		Log::Trace("", "", "cm_002001_rcv IN: st_no = [{0}]tqmts0x.FACTORY_DIV[{1}]", (const char*)tqmts0x.ST_NO, tqmts0x.FACTORY_DIV);
		
		/* ***** 检查输入参数合法性 ***** */
		if(tqmts0x.ST_NO.Trim() == "" || tqmts0x.ST_NO.Trim() == " ")
		{
			strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		/* ***** 程序处理 ***** */
		if(v_diff == 3)//删除
		{
			Log::Trace("", "","工艺卡删除");
			
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " DELETE FROM TQMTS0X "
							 "  WHERE ST_NO = @st_no "
							 " AND FACTORY_DIV =@tqmts0x.FACTORY_DIV" ;
					break;
			}
			cmd_del.SetCommandText(sqlstr);
			cmd_del.Parameters.Set("st_no", tqmts0x.ST_NO);
			cmd_del.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x.FACTORY_DIV);
			cmd_del.ExecuteNonQuery();
			cmd_del.Close();

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " DELETE FROM TQMTS02 "
							 "  WHERE ST_NO = @st_no "
							 " AND FACTORY_DIV =@tqmts0x.FACTORY_DIV";
					break;
			}
			cmd_del.SetCommandText(sqlstr);
			cmd_del.Parameters.Set("st_no", tqmts0x.ST_NO);
			cmd_del.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x.FACTORY_DIV);
			cmd_del.ExecuteNonQuery();
			cmd_del.Close();

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " DELETE FROM TQMTS01 "
							 "  WHERE ST_NO = @st_no "
							 " AND FACTORY_DIV =@tqmts0x.FACTORY_DIV";

					break;
			}
			cmd_del.SetCommandText(sqlstr);
			cmd_del.Parameters.Set("st_no", tqmts0x.ST_NO);
			cmd_del.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x.FACTORY_DIV);
			cmd_del.ExecuteNonQuery();
			cmd_del.Close();

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr_tep02 = "SELECT CODE_DESC_4_CONTENT FROM TEP0002 WHERE CODE_CLASS='QMZ7'"
					;
				break;
			}
			cmd_inq_tep02.SetCommandText(sqlstr);
			cmd_inq_tep02.ExecuteReader();
			while (cmd_inq_tep02.Read())
			{
				s_table_name = cmd_inq.GetString(1);
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " DELETE FROM TQMTS01 "
						"  WHERE ST_NO = @st_no "
						" AND FACTORY_DIV =@tqmts0x.FACTORY_DIV";
					break;
				}
				cmd_del.SetCommandText(sqlstr);
				cmd_del.Parameters.Set("st_no", tqmts0x.ST_NO);
				cmd_del.Parameters.Set("tqmts0x.FACTORY_DIV", tqmts0x.FACTORY_DIV);
				cmd_del.ExecuteNonQuery();
				cmd_del.Close();
			}
			cmd_inq_tep02.Close();
		}
		else if (v_diff == 1)////变更或追加
		
		{		
			Log::Trace("", "","工艺卡变更或追加");
			/* ***** 1、获取工序成分标准部分数据(非循环部分) ***** */
			tqmts0x.MergeFrom(bcls_rec->Tables[0].Rows[0]);	 

			/******************** 赋初值 *************************/
			tqmts0x.REC_CREATOR = "电文接收";
			tqmts0x.REC_CREATE_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts0x.REC_REVISOR = " ";
			tqmts0x.REC_REVISE_TIME = " ";
			tqmts0x.ARCHIVE_FLAG = " ";
			tqmts0x.DU_FLAG = " ";
			tqmts0x.DU_MAKER = " ";
			tqmts0x.DU_TIME = " ";
			tqmts0x.VERSION = 1;
			tqmts0x.VALID_FLAG = "1";//默认下发就是生效的
			tqmts0x.CHECK_TIME = " ";
			tqmts0x.CHECK_MAKER = " ";

			Log::Trace("", "","tqmts0x.ST_NO = [{0}]",(const char*)tqmts0x.ST_NO);

			tqmts0x.Delete("ST_NO,FACTORY_DIV"); //条件字段项

			tqmts0x.TrimOrBlank();
			tqmts0x.Insert();
			
			/* ***** 2、获取工序成分标准部分数据(循环部分) ***** */
			Log::Trace("", "","2、插入工序成分表");
			//删除工序成分
			tqmts02.ST_NO = tqmts0x.ST_NO;
			tqmts02.FACTORY_DIV = tqmts0x.FACTORY_DIV;
			Log::Trace("", "", "tqmts02.FACTORY_DIV = [{0}]", (const char*)tqmts02.FACTORY_DIV);
			Log::Trace("", "", "tqmts02.ST_NO = [{0}]", (const char*)tqmts02.ST_NO);
			Log::Trace("", "", "tqmts02.WHOLE_BACKLOG_CODE = [{0}]", (const char*)tqmts02.WHOLE_BACKLOG_CODE);
			if (tqmts0x.IDX_NO_ELM.Trim()=="") ////非挂索引方式，update by yiling 20160411
			{
				tqmts02.WHOLE_BACKLOG_CODE = "G";
				tqmts02.Delete("FACTORY_DIV,ST_NO,WHOLE_BACKLOG_CODE"); //条件字段项
			}
			else/////挂索引模式
			{
				tqmts02.Delete("FACTORY_DIV,ST_NO"); //条件字段项
			}


			//对输入信息循环处理
			for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
			{   
				//取得单行传入信息
				tqmts02.Reset();
				tqmts02.MergeFrom(bcls_rec->Tables[0].Rows[i]);
				tqmts02.ST_NO = bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();
				tqmts02.FACTORY_DIV = bcls_rec->Tables[0].Rows[0]["factory_div"].ToString().Trim();
				
				Log::Trace("", "","tqmts02.ST_NO = [{0}]",(const char*)tqmts02.ST_NO);
				Log::Trace("", "","tqmts02.ELM_CODE = [{0}]",(const char*)tqmts02.ELM_CODE);

				if(tqmts02.ELM_CODE.Trim() != "")
				{
					/******************** 赋初值 *************************/
					tqmts02.REC_CREATOR = "电文接收";
					tqmts02.REC_CREATE_TIME = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tqmts02.REC_REVISOR = " ";
					tqmts02.REC_REVISE_TIME = " ";
					tqmts02.ARCHIVE_FLAG = " "; 
					tqmts02.DU_FLAG = " ";
					tqmts02.DU_MAKER = " ";
					tqmts02.DU_TIME = " ";
					tqmts02.VERSION = 1;
					tqmts02.WHOLE_BACKLOG_SEQ = 0;			
					if (tqmts0x.IDX_NO_ELM.Trim() == "") ////非挂索引方式，update by yiling 20160411
					{
						tqmts02.WHOLE_BACKLOG_CODE = "G";
					}
					//获取元素顺序、元素单位——add by 冯晓轶 2012-03-23
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
									 "   AND CODE = @elm_code ";
							break;
					}
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("elm_code", tqmts02.ELM_CODE);
					cmd_inq.ExecuteReader();
					if(cmd_inq.Read())
					{
						cmd_inq.Fetch(tep0002);
						tqmts02.ELM_POS = CDecimal::Parse(tep0002.CODE_DESC_2_CONTENT);//得到元素顺序
						tqmts02.ELM_UNIT = "%";
					}
					cmd_inq.Close();
					
					//写入工艺成分
					tqmts02.TrimOrBlank();
					tqmts02.Insert();
				}
			}
		}
		else /////审核取消update by yiling ,挂索引模式采取审核及审核取消及删除这三个点发电文。
		{
			tqmts0x.VALID_FLAG = "0";
			tqmts0x.Update("VALID_FLAG","ST_NO,FACTORY_DIV");
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
