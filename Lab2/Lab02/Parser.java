// Parser.java
// Parser for language S
package Lab02;

public class Parser {
    Token token;          // current token 
    Lexer lexer;
    String funId = "";

    public Parser(Lexer scan) { 
        lexer = scan;		  
        token = lexer.getToken(); // get the first token
    }
  
    private String match(Token t) {
        String value = token.value();
        if (token == t)
            token = lexer.getToken();
        else
            error(t);
        return value;
    }

    private void error(Token tok) {
        System.err.println("Syntax error: " + tok + " --> " + token);
        token=lexer.getToken();
    }
  
    private void error(String tok) {
        System.err.println("Syntax error: " + tok + " --> " + token);
        token=lexer.getToken();
    }
  
    public Command command() {
    // <command> ->  <decl> | <function> | <stmt>
	    if (isType()) { //만약 타입이면 변수 선언
	        Decl d = decl();
	        return d;
	    }
	/*
	    if (token == Token.FUN) {
	        Function f = function();
	        return f;
	    }
	*/
	    if (token != Token.EOF) {
	        Stmt s = stmt();
            return s;
	    }
	    return null;
    }

    
    
    
    private Decl decl() { //변수 선언 객체
    // <decl>  -> <type> id [=<expr>]; 
        Type t = type(); //타입
	    String id = match(Token.ID); //match로 id가 나오는지 확인
	    Decl d = null;
	    if (token == Token.ASSIGN) { //expr 할당이 있으면...
	        match(Token.ASSIGN);
            Expr e = expr();
	        d = new Decl(id, t, e); //expr이 있으면 그거까지 포함해서 초기화
	    } else 
            d = new Decl(id, t); //할당이 없으면 없는대로 초기화

	    match(Token.SEMICOLON); //마지막에 세미콜론 있는지 확인
	    return d;
    }

    private Decls decls () { //변수 선언들은 변수 선언의 반복이다.
    // <decls> -> {<decl>}
        Decls ds = new Decls ();
	    while (isType()) { //타입이 안나올 떄까지
	        Decl d = decl();
	        ds.add(d);
	    }
        return ds;             
    }

/*
    private Function function() {
    // <function>  -> fun <type> id(<params>) <stmt> 
        match(Token.FUN);
	    Type t = type();
	    String str = match(Token.ID);
	    funId = str; 
	    Function f = new Function(str, t);
	    match(Token.LPAREN);
        if (token != Token.RPAREN)
            f.params = params();
	    match(Token.RPAREN);
	    Stmt s = stmt();		
	    f.stmt = s;
	    return f;
    }

    private Decls params() {
	    Decls params = new Decls();
        
		// parse declrations of parameters

        return params;
    }

*/

    private Type type () {
    // <type>  ->  int | bool | void | string 
        Type t = null;
        switch (token) { //타입이 int, bool, String 중 어떤 것인지 switch문으로 확인하는 것
	    case INT:
            t = Type.INT; break;
        case BOOL:
            t = Type.BOOL; break;
        case VOID:
            t = Type.VOID; break;
        case STRING:
            t = Type.STRING; break;
        default:
	        error("int | bool | void | string");
	    }
        match(token);
        return t;       
    }
  
    private Stmt stmt() { //stmt, 어떤 건지 확인하고 각 케이스로 순환
    // <stmt> -> <block> | <assignment> | <ifStmt> | <whileStmt> | ...
        Stmt s = new Empty();
        switch (token) {
	    case SEMICOLON:
            match(token.SEMICOLON); return s;
        case LBRACE:			
	        match(Token.LBRACE);		
            s = stmts();
            match(Token.RBRACE);	
	        return s;
        case IF: 	// if statement 
            s = ifStmt(); return s;
        case WHILE:      // while statement 
            s = whileStmt(); return s;
        case ID:	// assignment
            s = assignment(); return s;
	    case LET:	// let statement 
            s = letStmt(); return s;
	    case READ:	// read statement 
            s = readStmt(); return s;
	    case PRINT:	// print statment 
            s = printStmt(); return s;
	    case RETURN: // return statement 
            s = returnStmt(); return s;
        default:  
	        error("Illegal stmt"); return null; 
	    }
    }
  
    private Stmts stmts () { //복합문
    // <block> -> {<stmt>}
        Stmts ss = new Stmts();
	    while((token != Token.RBRACE) && (token != Token.END))
	        ss.stmts.add(stmt()); 
        return ss;
    }

    private Let letStmt () { //let문
    // <letStmt> -> let <decls> in <block> end
	    match(Token.LET);	
        Decls ds = decls();
	    match(Token.IN);
        Stmts ss = stmts();
        match(Token.END);	
        match(Token.SEMICOLON);
        return new Let(ds, null, ss);
    }

    // TODO: [Complete the code of readStmt()]
    private Read readStmt() {
    // <readStmt> -> read id;
    	match(Token.READ); // 'read'가 나오는지 확인
    	Identifier id = new Identifier(match(Token.ID));
    	match(Token.SEMICOLON);
    	return new Read(id); //
    // parse read statement 
    }

    // TODO: [Complete the code of printStmt()]
    private Print printStmt() {
    // <printStmt> -> print <expr>;
    	match(Token.PRINT); // "print"가 나오는지 확인
        Expr e = expr(); // expr을 읽어서 Expr 객체 생성
        match(Token.SEMICOLON); // 세미콜론 확인 (반환 전)
        return new Print(e);
    }

    private Return returnStmt() { //리턴문? 
    // <returnStmt> -> return <expr>; 
        match(Token.RETURN);
        Expr e = expr();
        match(Token.SEMICOLON);
        return new Return(funId, e);
    }

    private Stmt assignment() { //할당문
    // <assignment> -> id = <expr>;   
        Identifier id = new Identifier(match(Token.ID)); 
        //Id는 토큰에서 ID가 맞는지 확인(문자로 시작하고 예약어가 아니면 변수 맞음) 이름 초기화하고 생성
	/*
	    if (token == Token.LPAREN)    // function call 
	        return call(id);
	*/

        match(Token.ASSIGN); //할당이 있는 지 확인
        Expr e = expr(); // '='이 있으면 expr 생성
        match(Token.SEMICOLON); //세미콜론이 확인되면
        return new Assignment(id, e); //변수에 할당
    }

/*
    private Call call(Identifier id) {
    // <call> -> id(<expr>{,<expr>});
    //
    // parse function call
    //
	return null;
    }
*/

    private If ifStmt () { //if문
    // <ifStmt> -> if (<expr>) then <stmt> [else <stmt>]
        match(Token.IF);
	    match(Token.LPAREN);
        Expr e = expr();
	    match(Token.RPAREN);
        match(Token.THEN);
        Stmt s1 = stmt();
        Stmt s2 = new Empty();
        if (token == Token.ELSE){
            match(Token.ELSE); 
            s2 = stmt();
        }
        return new If(e, s1, s2);
    }

    // TODO: [Complete the code of whileStmt()]
    private While whileStmt () {
    // <whileStmt> -> while (<expr>) <stmt>
    match(Token.WHILE); //while 등장
    match(Token.LPAREN); //왼쪽 괄호
    Expr e = expr(); //수식
    match(Token.RPAREN); //오른쪽 괄호
    Stmt s = stmt(); //구문
    return new While(e,s); //while 리턴
    }

    private Expr expr () {
    // <expr> -> <bexp> {& <bexp> | '|'<bexp>} | !<expr> | true | false
        switch (token) {
	    case NOT:
	        Operator op = new Operator(match(token));
	        Expr e = expr();
            return new Unary(op, e);
        case TRUE:
            match(Token.TRUE);
            return new Value(true);
        case FALSE:
            match(Token.FALSE);
            return new Value(false);
        }

        Expr e = bexp();
        // TODO: [Complete the code of logical operations for <expr> -> <bexp> {& <bexp> | '|'<bexp>}]
        while (token == Token.AND || token == Token.OR) {
            Operator op = null;

            if (token == Token.AND) {
                op = new Operator("&");
                match(Token.AND);  // AND 연산자 매칭
            } else if (token == Token.OR) {
                op = new Operator("|");
                match(Token.OR);  // OR 연산자 매칭
            }

            // 다음 bexp를 파싱하여 논리 연산 적용
            Expr e2 = bexp();
            e = new Binary(op, e, e2);  // 왼쪽과 오른쪽 bexp를 연산자로 결합
        }

        return e;  // 최종 표현식 반환
		// parse logical operations
    }

    // TODO: [Complete the code of bexp()]
    private Expr bexp() {
        // <bexp> -> <aexp> [ (< | <= | > | >= | == | !=) <aexp> ]
    	//bexp의 순환
    	Expr e = aexp();
    	// 관계 연산자(<, <=, >, >=, ==, !=) 처리
    	Operator op = null;
    	switch (token) {
    		case LT:
    			op = new Operator("<");
    	 		match(Token.LT);  // '<' 연산자 매칭
    	 		break;
    	 	case LTEQ:
    	 		op = new Operator("<=");
    	 		match(Token.LTEQ);  // '<=' 연산자 매칭
    	 		break;
    	 	case GT:
    	 		op = new Operator(">");
    	 		match(Token.GT);  // '>' 연산자 매칭
    	 		break;
    	    case GTEQ:
    	        op = new Operator(">=");
    	        match(Token.GTEQ);  // '>=' 연산자 매칭
    	        break;
    	    case EQUAL:
    	    	op = new Operator("==");
    	    	match(Token.EQUAL);  // '==' 연산자 매칭
    	    	break;
    	    case NOTEQ:
    	    	op = new Operator("!=");
    	    	match(Token.NOTEQ);  // '!=' 연산자 매칭
    	    	break;
    	 }    
    	 if(op != null) {
    		 Expr e2 = aexp();
    	     e = new Binary(op, e, e2);
    	 }
    	 return e;  // 최종 표현식 반환
	// parse relational operations
    }
  
    private Expr aexp () { //aexp는 이미 구현됨.
        // <aexp> -> <term> { + <term> | - <term> }
        Expr e = term(); //term 실행
        while (token == Token.PLUS || token == Token.MINUS) { // +/- 판단
            Operator op = new Operator(match(token));
            Expr t = term(); //op 반환후 뒤에 나오는 term 구현
            e = new Binary(op, e, t);  //구현후 초기화
        }
        return e;
    }
  
    private Expr term () {
        // <term> -> <factor> { * <factor> | / <factor>}
        Expr t = factor();
        while (token == Token.MULTIPLY || token == Token.DIVIDE) {
            Operator op = new Operator(match(token));
            Expr f = factor();
            t = new Binary(op, t, f);
        }
        return t;
    }
  
    private Expr factor() {
        // <factor> -> [-](id | <call> | literal | '('<aexp> ')')
        Operator op = null;
        if (token == Token.MINUS) 
            op = new Operator(match(Token.MINUS));

        Expr e = null;
        switch(token) {
        case ID:
            Identifier v = new Identifier(match(Token.ID));
            e = v;
            if (token == Token.LPAREN) {  // function call
                match(Token.LPAREN); 
                Call c = new Call(v,arguments());
                match(Token.RPAREN);
                e = c;
            } 
            break;
        case NUMBER: case STRLITERAL: 
            e = literal();
            break; 
        case LPAREN: 
            match(Token.LPAREN); 
            e = aexp();       
            match(Token.RPAREN);
            break; 
        default: 
            error("Identifier | Literal"); 
        }

        if (op != null)
            return new Unary(op, e);
        else return e;
    }
  
    private Exprs arguments() {
    // arguments -> [ <expr> {, <expr> } ]
        Exprs es = new Exprs();
        while (token != Token.RPAREN) {
            es.add(expr());
            if (token == Token.COMMA)
                match(Token.COMMA);
            else if (token != Token.RPAREN)
                error("Exprs");
        }  
        return es;  
    }

    private Value literal( ) {
        String s = null;
        switch (token) {
        case NUMBER:
            s = match(Token.NUMBER);
            return new Value(Integer.parseInt(s));
        case STRLITERAL:
            s = match(Token.STRLITERAL);
            return new Value(s);
        }
        throw new IllegalArgumentException( "no literal");
    }
 
    private boolean isType( ) {
        switch(token) {
        case INT: case BOOL: case STRING: 
            return true;
        default: 
            return false;
        }
    }
    
    public static void main(String args[]) {
	    Parser parser;
        Command command = null;
	    if (args.length == 0) {
	        System.out.print(">> ");
	        Lexer.interactive = true;
	        parser  = new Parser(new Lexer());
	        do {
	            if (parser.token == Token.EOF) 
		        parser.token = parser.lexer.getToken();

                try {
                    command = parser.command();
		            if (command != null) command.display(0);    // display AST, TODO: [Uncomment this line]
                } catch (Exception e) {
                    System.err.println(e);
                }
		        System.out.print("\n>> ");
	        } while(true);
	    }
    	else {
	        System.out.println("Begin parsing... " + args[0]);
	        parser  = new Parser(new Lexer(args[0]));
	        do {
	            if (parser.token == Token.EOF)
                    break;
                try {
		             command = parser.command();
		             if (command != null) command.display(0);      // display AST, TODO: [Uncomment this line]
                } catch (Exception e) {
                    System.err.println(e); 
                }
	        } while (command != null);
	    }
    } //main
} // Parser