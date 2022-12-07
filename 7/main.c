
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 128
#define MAXPATH 128
#define MAXCHILDREN 128

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef enum NodeType
{
    NodeDirectory,
    NodeFile
} NodeType;

typedef struct TreeNode TreeNode;
struct TreeNode
{
    NodeType type;
    char name[MAXPATH];
    size_t fileSize;
    TreeNode* children[MAXCHILDREN];
    size_t numChildren;
    TreeNode* parent;
};

TreeNode* createNodeDirectory(const char* name) {
    TreeNode* n = malloc(sizeof(TreeNode));
    n->type = NodeDirectory;
    strncpy(n->name, name, MIN(strlen(name), MAXPATH));
    n->fileSize = 0;
    for(size_t i = 0; i < MAXCHILDREN; ++i) {
        n->children[0] = NULL;
    }
    n->numChildren = 0;
    n->parent = NULL;
    return n;
}
TreeNode* createNodeFile(const char* name, size_t size) {
    TreeNode* n = malloc(sizeof(TreeNode));
    n->type = NodeFile;
    strncpy(n->name, name, MIN(strlen(name), MAXPATH));
    n->fileSize = size;
    for(size_t i = 0; i < MAXCHILDREN; ++i) {
        n->children[0] = NULL;
    }
    n->numChildren = 0;
    n->parent = NULL;
    return n;
}
void directoryAddChild(TreeNode* parent, TreeNode* child) {
    parent->children[parent->numChildren] = child;
    parent->numChildren++;
    child->parent = parent;
}
TreeNode* directoryFindChild(TreeNode* parent, const char* childName) {
    for( size_t i = 0; i < parent->numChildren; ++i ) {
        TreeNode* child = parent->children[i];
        if( strcmp(child->name, childName) == 0 ) {
            return child;
        }
    }
    return NULL;
}
void destroyNodeRecursive(TreeNode* node) {
    if( node == NULL ) return;
    for(size_t i = 0; i < MAXCHILDREN; ++i) {
        destroyNodeRecursive(node->children[i]);
    }
    free(node);
}

void parseLineAndBuildTree(char* line, TreeNode** root, TreeNode** pwd) {
    
    if(line == NULL) {
        printf("Invalid Input\n");
        return;
    }
    size_t length = strlen(line);
    if( line[length - 1] == '\n' ) {
        line[length - 1] = '\0';
        length = strlen(line);
    }

    // printf("%s\n", line);

    if( *root == NULL ) {
        if( strcmp(line, "$ cd /") != 0 ) {
            printf("First line in input is bad!\n");
            exit(99);
        }
        *root = createNodeDirectory("/");
        *pwd = *root;
    }
    else if( line[0] == '$' ) {
        char* command = strtok(line + 2, " ");

        if( strcmp(command, "cd") == 0 ) {
            char* arg = strtok(NULL, " ");
            if( strcmp(arg, "..") == 0 ) {
                *pwd = (*pwd)->parent;
            } else if( arg != NULL && pwd != NULL ) {
                TreeNode* child = directoryFindChild(*pwd, arg);
                if( child == NULL ) {
                    printf("Failed to find child %s\n", arg);
                    exit(1);
                }
                else if( child->type != NodeDirectory ) {
                    printf("Cannot cd to file %s\n", arg);
                    exit(1);
                }
                *pwd = child;
            } else {
                printf("Can't cd without pwd and a child\n");
                exit(1);
            }
        }
        else if( strcmp(command, "ls") ) {
            // TODO: Only commands are dir and ls (so far)
            //       so just continue to next line and assume any non-commands are ls ;)
            return;
        }
    } else {
        if( pwd == NULL ) {
            printf("Can't ls without PWD\n");
            exit(1);
        }
        // ls output
        if( strncmp(line, "dir ", 4) == 0 ) {
            TreeNode* childDir = createNodeDirectory(line + 4);
            directoryAddChild(*pwd, childDir);
        } else {
            char* fileSizeStr = strtok(line, " ");
            char* fileName = NULL;
            if( fileSizeStr != NULL ) {
                fileName = strtok(NULL, " ");
            }

            if( fileSizeStr == NULL || fileName == NULL ) {
                printf("Failed to parse file line\n");
                exit(1);
            }

            size_t fileSize = atoi(fileSizeStr);
            TreeNode* childFile = createNodeFile(fileName, fileSize);
            directoryAddChild(*pwd, childFile);
        }
    }
}

void printTree(TreeNode* node, int indent) {
    for(int j = 0; j < indent; ++j) printf(" ");
    if( node->type == NodeDirectory ) {
        printf("%s (dir)\n", node->name);
        indent += 2;
        for(size_t i = 0; i < node->numChildren; ++i) {
            // for(int j = 0; j < indent; ++j) printf(" ");
            printTree(node->children[i], indent);
        }
        indent -= 2;
    } else if( node->type == NodeFile ) {
        printf("%s (file, size=%lu)\n", node->name, node->fileSize);
    }
}

size_t directorySize(TreeNode* node) {
    if( node == NULL ) return 0;
    size_t s = 0;
    for( size_t i = 0; i < node->numChildren; ++i ) {
        TreeNode* child = node->children[i];
        if( child->type == NodeFile ) {
            s += child->fileSize;
        } else if(child->type == NodeDirectory ) {
            s += directorySize(child);
        }
    }
    return s;
}

size_t sumDirectorySizesWhereLessThanOrEqualTo(TreeNode* node, size_t maxDirSize) {    
    size_t s = 0;

    size_t nodeSize = directorySize(node);
    if( nodeSize <= maxDirSize ) s += nodeSize;

    for( size_t i = 0; i < node->numChildren; ++i ) {
        TreeNode* child = node->children[i];
        s += sumDirectorySizesWhereLessThanOrEqualTo(child, maxDirSize);
    }
    return s;
}

TreeNode* findSmallestDirectoryWithTotalSizeAbove(TreeNode* node, size_t threshold) {
    TreeNode* foundNode = NULL;
    
    if( directorySize(node) > threshold ) {
        if( foundNode == NULL ) {
            foundNode = node;
        }
        else if( directorySize(node) < directorySize(foundNode) ) {
            foundNode = node;
        }
    }

    for( size_t i = 0; i < node->numChildren; ++i ) {
        TreeNode* child = node->children[i];
        if( child->type == NodeDirectory ) {
            TreeNode* x = findSmallestDirectoryWithTotalSizeAbove(child, threshold);

            if( directorySize(x) > threshold ) {
                if( foundNode == NULL ) {
                    foundNode = x;
                }
                else if( directorySize(x) < directorySize(foundNode) ) {
                    foundNode = x;
                }
            }
        }
    }

    return foundNode;
}


size_t problem1(FILE *f)
{
    char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    TreeNode* root = NULL;
    TreeNode* pwd = NULL;

    while (fgets(buffer, MAXLEN, f))
    {
        // printf("line: %s\n", buffer);
        parseLineAndBuildTree(buffer, &root, &pwd);
    }

    // printTree(root, 0);

    // Sum of directory sizes, where directory size is <= 100000
    size_t v = sumDirectorySizesWhereLessThanOrEqualTo(root, 100000);

    // TODO: destroy is broken :[
    // destroyNodeRecursive(root);
    return v;
}

size_t problem2(FILE *f)
{
    char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    TreeNode* root = NULL;
    TreeNode* pwd = NULL;

    while (fgets(buffer, MAXLEN, f))
    {
        // printf("line: %s\n", buffer);
        parseLineAndBuildTree(buffer, &root, &pwd);
    }

    // printTree(root, 0);

    size_t totalDiskSpace = 70000000;
    size_t freeSpaceNeeded = 30000000;
    size_t spaceToFree = freeSpaceNeeded - (totalDiskSpace - directorySize(root));
    printf("Finding directory to free at least %lu\n", spaceToFree);
    TreeNode* dirToDelete = findSmallestDirectoryWithTotalSizeAbove(root, spaceToFree);
    printf("We should delete directory %s\n", dirToDelete->name);
    size_t v = directorySize(dirToDelete);

    // TODO: destroy is broken :[
    // destroyNodeRecursive(root);
    return v;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("USAGE: %s input.txt\n", argv[0]);
        return 1;
    }

    const char *inputFilePath = argv[1];

    FILE *inputFile = fopen(inputFilePath, "r");
    if (!inputFile)
    {
        printf("Error: Failed to open file: %s", inputFilePath);
        return 1;
    }

    printf("Problem 1: %lu\n", problem1(inputFile));
    fseek(inputFile, 0, SEEK_SET);
    printf("Problem 2: %lu\n", problem2(inputFile));

    fclose(inputFile);

    return 0;
}
