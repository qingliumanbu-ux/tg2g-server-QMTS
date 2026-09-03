//Dec 16 08:33//请不要修改此行

#include "stdafx.h"

BM2F_ENTERACE(qmtiz3_inq);

int f_qmtiz3_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//系统日志类定义
	CTracer log(__FUNCTION__);	

	//程序用变量
	int  doFlag = 0;
	CString q_mat_no = " ";
	CString q_order_no = " ";
	CString q_program_name = " ";
	CString q_rec_create_time_from = " ";
	CString q_rec_create_time_to = " ";
	CString q_err_flag = " ";
	CString q_pono =" ";
	CString q_rl_no =" ";
	CString q_rl_no_certi =" ";

	try
	{
		//接收从前台传入的值
		q_mat_no    = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		q_order_no    = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().Trim();
		q_program_name    = bcls_rec->Tables[0].Rows[0]["PROGRAM_NAME"].ToString().Trim();
		q_rec_create_time_from    = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_FROM"].ToString().Trim();
		q_rec_create_time_to    = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_TO"].ToString().Trim();
		q_err_flag    = bcls_rec->Tables[0].Rows[0]["ERR_FLAG"].ToString().Trim();
		q_pono        = bcls_rec->Tables[0].Rows[0]["PONO"].ToString().Trim();
		q_rl_no       = bcls_rec->Tables[0].Rows[0]["RL_NO"].ToString().Trim();
		q_rl_no_certi = bcls_rec->Tables[0].Rows[0]["RL_NO_CERTI"].ToString().Trim();

		//拼接查询条件
		CDbCommand cmd(conn);
		CDbCommand cmd1(conn);
		CString sql_where = "";
		if (!q_mat_no.Trim().IsEmpty())
		{
			sql_where += " AND MAT_NO  LIKE @mat_no||'%' ";
			cmd.Parameters.Set("mat_no", q_mat_no);
			cmd1.Parameters.Set("mat_no", q_mat_no);
		}
		if (!q_order_no.Trim().IsEmpty())
		{
			sql_where += " AND ORDER_NO  LIKE @order_no||'%' ";
			cmd.Parameters.Set("order_no", q_order_no);
			cmd1.Parameters.Set("order_no", q_order_no);
		}
		if (!q_program_name.Trim().IsEmpty())
		{
			sql_where += " AND PROGRAM_NAME LIKE @program_name||'%' ";
			cmd.Parameters.Set("program_name", q_program_name);
			cmd1.Parameters.Set("program_name", q_program_name);
		}
		if (!q_rec_create_time_from.Trim().IsEmpty())
		{
			sql_where += " AND REC_CREATE_TIME >= @rec_create_time_from || ' ' ";
			cmd.Parameters.Set("rec_create_time_from", q_rec_create_time_from);
			cmd1.Parameters.Set("rec_create_time_from", q_rec_create_time_from);
		}
		if (!q_rec_create_time_to.Trim().IsEmpty())
		{
			sql_where += " AND REC_CREATE_TIME <= @rec_create_time_to || '999999' ";
			cmd.Parameters.Set("rec_create_time_to", q_rec_create_time_to);
			cmd1.Parameters.Set("rec_create_time_to", q_rec_create_time_to);
		}
		if (!q_err_flag.Trim().IsEmpty())
		{
			sql_where += " AND ERR_FLAG LIKE @ERR_FLAG||'%' ";
			cmd.Parameters.Set("ERR_FLAG", q_err_flag);
			cmd1.Parameters.Set("ERR_FLAG", q_err_flag);
		}
		if (!q_pono.Trim().IsEmpty())
		{
			sql_where += " AND PONO LIKE @pono||'%' ";
			cmd.Parameters.Set("pono", q_pono);
			cmd1.Parameters.Set("pono", q_pono);
		}
		if (!q_rl_no.Trim().IsEmpty())
		{
			sql_where += " AND RL_NO LIKE @rl_no||'%' ";
			cmd.Parameters.Set("rl_no", q_rl_no);
			cmd1.Parameters.Set("rl_no", q_rl_no);
		}
		if (!q_rl_no_certi.Trim().IsEmpty())
		{
			sql_where += " AND RL_NO_CERTI LIKE @rl_no_certi||'%' ";
			cmd.Parameters.Set("rl_no_certi", q_rl_no_certi);
			cmd1.Parameters.Set("rl_no_certi", q_rl_no_certi);
		}

        //获取总条数
		CString sql_count = " SELECT count(1) FROM TQMTIZ0 WHERE 1=1 " + sql_where;
		cmd.SetCommandText(sql_count);
	    CDecimal rc=cmd.ExecuteScalar();

		//把值压入RC中，传出前台
        bcls_ret->ExtendedProperties.Add("RC",rc.ToString());

		//起始-终止之间
		int nStar=(int)bcls_rec->Tables[1].Rows[0]["START"];
		int nPageSize=(int)bcls_rec->Tables[1].Rows[0]["PAGE_SIZE"];
		CString sqlstr = " SELECT * FROM TQMTIZ0 WHERE 1=1 " + sql_where;
		cmd1.SetCommandText(sqlstr);

		int count=cmd1.ExecuteQuery(bcls_ret->Tables[0],nStar,nPageSize);
		bcls_ret->Tables[0].set_TableName("QMTIZ3_INQ");
		/*.print();*/
		sprintf(s.msg,"查询成功！");

	}

	//捕获数据库操作异常
	catch(CDbException& ex)  
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = ex.GetMsg();

		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;

		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}

	//try块程序发生异常
	catch(const CApplicationException& ex)
	{
		doFlag = ex.GetCode();
		sprintf(s.msg,"%s:%s",(const char*)ex.GetSource(),(const char*)ex.GetMsg());
	}

	//平台异常
	catch(const CException& ex)
	{
		doFlag = -1;
		strcpy(s.msg, (const char*)ex.GetMsg());
	}

	s.flag = doFlag;

	return(doFlag);
}