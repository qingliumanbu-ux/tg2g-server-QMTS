/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:       
Version:     1.0
Date:        2023-02-6
Description: 修改
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中

/*<remark>=========================================================
/// <summary>
/// 修改制造标准
/// <para>
/// 获取输入参数：
/// </para>
/// <para>数据库表：TQMTS6C(降级改判表) /// 	
///       </para>
/// </summary>
/// <param name="TQMTS6C">降级改判表    </param>
===========================================================</remark>*/ 

// service入口
BM2F_ENTERACE(qmts6c_upd)


int f_qmts6c_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	int flag = 0;

	CString table_name = " ";
	CString q_st_no = " ";
	CString q_whole_backlog_code = " ";
	CDecimal v_count = 0;
	CString s_equ_no = " ";
	CString s_ingot_code = " ";
	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	/* 实体类定义 */
	CModel tqmts6c("TQMTS6C");
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_01(conn);

	try
	{
		int count = bcls_rec->Tables[0].Rows.get_Count();

		for (int i = 0; i < count; i++)
		{
			tqmts6c.Reset();
			tqmts6c.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			//table_name = bcls_rec->Tables[1].Rows[0]["table_name"].ToString().ToLower().Trim();
			//q_st_no = bcls_rec->Tables[1].Rows[0]["st_no"].ToString().Trim();
			//q_whole_backlog_code = bcls_rec->Tables[1].Rows[0]["whole_backlog_code"].ToString().Trim();
			//flag = bcls_rec->Tables[1].Rows[0]["flag"].ToDecimal().ToInt16();
			//tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[1].Rows[0]["factory_div"].ToString().TrimOrBlank();  ////update by yiling 20170224tqmts02["REC_REVISOR"] = s.userid;
			tqmts6c["REC_REVISOR"] = s.userid;
			tqmts6c["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			//tqmtma8["VERSION"] = tqmtma8["VERSION"].ToDecimal() + 1;

			//修改
			//REC_CREATOR	REC_CREATE_TIME	REC_REVISOR	REC_REVISE_TIME	ARCHIVE_FLAG	DU_FLAG	DU_MAKER	DU_TIME	VERSION	CATCH_RESP	CATCH_TIME	MESSAGE_RESP	MESSAGE_TIME	RES_CODE	ABN_CONT	TC_TRANS_TIME	BASE_CODE	BASE_NAME	CODE_LINE	ST_NO_PLAN	SEQ_NO	ST_NO_ALLOW	SLAB_NUM	ST_NO_JJ	MEMO_ITEM	WIDTH_MAX	WIDTH_MIN	SLAB_DEST	SLAB_NUM_1

			tqmts6c.Update("REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME,ARCHIVE_FLAG,DU_FLAG,DU_MAKER,DU_TIME,VERSION,CATCH_RESP,CATCH_TIME,MESSAGE_RESP,MESSAGE_TIME,RES_CODE,ABN_CONT,TC_TRANS_TIME,BASE_CODE,BASE_NAME,CODE_LINE,ST_NO_PLAN,SEQ_NO,ST_NO_ALLOW,SLAB_NUM,ST_NO_JJ,MEMO_ITEM,WIDTH_MAX,WIDTH_MIN,SLAB_DEST,SLAB_NUM_1");
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
