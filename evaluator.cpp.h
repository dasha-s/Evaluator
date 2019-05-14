// Public routine that performs the evaluation. Examines the postfix machine
// to see if a single result is left and if so, return it; otherwise prints
// error.
template<class NumericType>
NumericType Evaluator<NumericType>::getValue( ) {

    Tokenizer<NumericType> tok( str );
    Token<NumericType> lastToken;

    do {
        lastToken = tok.getToken( );
        processToken( lastToken );
    } while ( lastToken.getType( ) != EOL );

    if ( postFixStack.empty( ) ) {
        cerr << "Missing operatnd!" << endl;
        return 0;
    }

    //everytime the value is popped from one stack, it needs to be popped from the other
    NumericType theResult = postFixStack.back( );
    postFixStack.pop_back( );
    postFixVarStack.pop_back();
    if ( !postFixStack.empty( ) )
        cerr << "Warning: missing operators!" << endl;

    return theResult;
}

// After token is read, use operator precedence parsing algorithm to process
// it; missing opening parentheses are detected here.
template<class NumericType>
void Evaluator<NumericType>::
processToken( const Token<NumericType> &lastToken ) {
    TokenType topOp;
    TokenType lastType = lastToken.getType( );

    switch( lastType ) {
        case VALUE:
            postFixStack.push_back( lastToken.getValue( ) );
            postFixVarStack.push_back( ' ' );
            return;
        case VAR_A:
            postFixStack.push_back( var_a );
            postFixVarStack.push_back( 'a' );
            return;
        case VAR_B:
            postFixStack.push_back( var_b );
            postFixVarStack.push_back( 'b' );
            return;
        case VAR_C:
            postFixStack.push_back( var_c );
            postFixVarStack.push_back( 'c' );
            return;
        case CPAREN:
            while( ( topOp = opStack.back( ) ) != OPAREN &&
                   topOp != EOL )
                performOp( topOp );
            if ( topOp == OPAREN )
                opStack.pop_back( ); // get rid of opening parethesis
            else
                cerr << "Missing open parenthesis" << endl;
            break;

        default: // general operator case
            while ( PREC_TABLE[ lastType ].inputSymbol <=
                    PREC_TABLE[ topOp = opStack.back( ) ].topOfStack )
                performOp( topOp );

            if ( lastToken.getType( ) != EOL )
                opStack.push_back( lastType );

            break;
    }
}

// Determine if there's a binary or unary operation
template<class NumericType>
void Evaluator<NumericType>::performOp( TokenType topOp ) {
    if ( topOp == OPAREN ) {
        cerr << "Unbalanced parentheses" << endl;
        opStack.pop_back( );
        return;
    }
    //split by unary or binary operations
    if ( topOp == NOT || topOp == BIT_COMP || topOp == UN_MINUS || topOp == UN_PLUS ) {
        unaryOp(topOp);
    } else {
        binaryOp(topOp);
    }
}

// Process an operator by taking two items off the postfix stack, applying
// the operator, and pushing the result.
// Print error if missing closing parenthesis or division by 0.
template<class NumericType>
void Evaluator<NumericType>::binaryOp( TokenType topOp ) {
    char var;
    NumericType rhs = getTop(var);
    NumericType lhs = getTop(var);

    // the original operators
    if (topOp == PLUS) {
        postFixStack.push_back(lhs + rhs);
        postFixVarStack.push_back(' ');   //update the stack simultaneously with the postFixStack
    }
    else if (topOp == MINUS) {
        postFixStack.push_back(lhs - rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == MULT) {
        postFixStack.push_back(lhs * rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == DIV) {
        if (rhs != 0) {
            postFixStack.push_back(lhs / rhs);
            postFixVarStack.push_back(' ');

        } else {
            cerr << "Division by zero" << endl;
            postFixStack.push_back(lhs);
            postFixVarStack.push_back(' ');
        }
    }

        // C++ operators
    else if (topOp == MODULUS) {
        postFixStack.push_back(lhs % rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == SHIFT_L) {
        postFixStack.push_back(lhs << rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == SHIFT_R) {
        postFixStack.push_back(lhs >> rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == LT) {
        postFixStack.push_back(lhs < rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == LE) {
        postFixStack.push_back(lhs <= rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == GT) {
        postFixStack.push_back(lhs > rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == GE) {
        postFixStack.push_back(lhs >= rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == EQUAL) {
        postFixStack.push_back(lhs == rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == NOTEQUAL) {
        postFixStack.push_back(lhs != rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == BIT_AND) {
        postFixStack.push_back(lhs & rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == BIT_EOR) {
        postFixStack.push_back(lhs ^ rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == BIT_IOR) {
        postFixStack.push_back(lhs | rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == LOG_AND) {
        postFixStack.push_back(lhs && rhs);
        postFixVarStack.push_back(' ');
    }
    else if (topOp == LOG_OR) {
        postFixStack.push_back(lhs || rhs);
        postFixVarStack.push_back(' ');
    }
    //assignment operator
    else if (topOp == ASSIGN ) {
        switch (var) {
            case 'a':
                var_a = rhs;   //update variables
                //push stacks
                postFixStack.push_back(rhs);
                postFixVarStack.push_back('a');   //push the variable as a result
                break;
            case 'b':
                var_b = rhs;
                postFixStack.push_back(rhs);
                postFixVarStack.push_back('b');
                break;
            case 'c':
                var_c = rhs;
                postFixStack.push_back(rhs);
                postFixVarStack.push_back('c');
                break;
            default:
                cerr << "Error while trying to handle variables" << endl;  //error if there's an incorrect variable name
                break;
        }
    }

    opStack.pop_back( );
}

// Process an unary operator by taking one item off the postfix stack, applying
// the operator, and pushing the result.
template<class NumericType>
void Evaluator<NumericType>::unaryOp( TokenType topOp ) {
    char var;
    NumericType operand = getTop(var);
    //There're 4 unary operators
    if ( topOp == NOT ) {
        postFixStack.push_back(!operand);
        postFixVarStack.push_back(' ');
    }
    else if ( topOp == BIT_COMP ) {
        postFixStack.push_back(~operand);
        postFixVarStack.push_back(' ');
    }
    else if ( topOp == UN_MINUS ) {
        postFixStack.push_back(-operand);
        postFixVarStack.push_back(' ');
    }
    else if ( topOp == UN_PLUS ) {
        postFixStack.push_back(+operand);
        postFixVarStack.push_back(' ');
    }

    opStack.pop_back( );
}

// top and pop the postfix machine stack; return the result.
// If the stack is empty, print an error message.
template<class NumericType>
NumericType Evaluator<NumericType>::getTop(char &var ) {
    if ( postFixStack.empty( ) ) {
        cerr << "Missing operand" << endl;
        return 0;
    }
    //remove the elements from the tops of the both stacks
    NumericType tmp = postFixStack.back();
    postFixStack.pop_back( );
    var = postFixVarStack.back();
    postFixVarStack.pop_back();
    return tmp;
}

